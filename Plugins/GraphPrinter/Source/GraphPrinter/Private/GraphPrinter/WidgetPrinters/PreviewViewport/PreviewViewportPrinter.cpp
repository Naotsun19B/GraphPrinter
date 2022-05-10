// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/PreviewViewport/PreviewViewportPrinter.h"
#include "GraphPrinter/Utilities/WidgetPrinterUtils.h"
#include "GraphPrinter/Utilities/CastSlateWidget.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet2/DebuggerCommands.h"
#include "Framework/Docking/TabManager.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "SGraphEditorImpl.h"

#define LOCTEXT_NAMESPACE "PreviewViewportPrinter"

UTextureRenderTarget2D* UPreviewViewportPrinter::GetPrintResult(
	const TSharedPtr<SWidget>& TargetWidget,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	auto* This = GetMutableDefault<UPreviewViewportPrinter>();
	if (!TargetWidget.IsValid() || !IsValid(This))
	{
		return nullptr;
	}

	EVisibility PreviousVisibility;
	This->PrePrintPreviewViewport(PreviousVisibility, TargetWidget, Options);

	FVector2D DrawSize;
	{
		const FGeometry& Geometry =
#if BEFORE_UE_4_23
			TargetWidget->GetCachedGeometry();
#else
			TargetWidget->GetTickSpaceGeometry();
#endif

		DrawSize = Geometry.GetAbsoluteSize();
	}
	
	const bool bIsPrintableSize = This->IsPrintableSize(TargetWidget, DrawSize, Options);

	UTextureRenderTarget2D* RenderTarget = nullptr;
	if (bIsPrintableSize)
	{
		RenderTarget = This->DrawWidgetToRenderTarget(
			TargetWidget,
			DrawSize,
			Options
		);
	}

	This->PostPrintPreviewViewport(TargetWidget, PreviousVisibility, Options);

	return RenderTarget;
}

void UPreviewViewportPrinter::PrintWidget(GraphPrinter::FPrintWidgetOptions Options)
{
	const TSharedPtr<SWidget> PreviewViewport = FindPreviewViewport(Options.TargetWidget);

	UTextureRenderTarget2D* RenderTarget = GetPrintResult(PreviewViewport, Options);
	if (!IsValid(RenderTarget))
	{
		return;
	}
	
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	PrepareCopyToClipboard(Options);
#endif
	
	FString Filename = CreateFilename(PreviewViewport, Options);
	
	TWeakObjectPtr<UPreviewViewportPrinter> WeakThis = this;
	Options.ImageWriteOptions.NativeOnComplete = [WeakThis, Filename, Options](bool bIsSucceeded)
	{
		if (!WeakThis.IsValid() || !bIsSucceeded)
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("FailedOutputError", "Failed capture preview viewport."),
				GraphPrinter::FGraphPrinterUtils::CS_Fail
			);
			return;
		}

		if (Options.ExportMethod == GraphPrinter::EExportMethod::ImageFile)
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("SucceededOutput", "Preview viewport capture saved as"),
				GraphPrinter::FGraphPrinterUtils::CS_Success, 5.f,
				TArray<GraphPrinter::FNotificationInteraction>{
					GraphPrinter::FNotificationInteraction(
						FText::FromString(Filename),
						FSimpleDelegate::CreateLambda([Filename]()
						{
							GraphPrinter::FGraphPrinterUtils::OpenFolderWithExplorer(Filename);
						})
					)
				}
			);
		}
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		else if (Options.ExportMethod == GraphPrinter::EExportMethod::Clipboard)
		{
			if (WeakThis->CopyImageFileToClipboard(Filename, Options))
			{
				GraphPrinter::FGraphPrinterUtils::ShowNotification(
					LOCTEXT("SucceededClipboardCopy", "Succeeded to copy image to clipboard."),
					GraphPrinter::FGraphPrinterUtils::CS_Success
				);
			}
			else
			{
				GraphPrinter::FGraphPrinterUtils::ShowNotification(
					LOCTEXT("FailedClipboardCopy", "Failed to copy image to clipboard."),
					GraphPrinter::FGraphPrinterUtils::CS_Fail
				);
			}

			IFileManager::Get().Delete(*Filename, false, true);
		}
#endif
	};
	
	ExportRenderTargetToImageFile(RenderTarget, Filename, Options);
}

bool UPreviewViewportPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	if (Options.PrintScope == GraphPrinter::EPrintScope::Selected)
	{
		return false;
	}
	
	const TSharedPtr<SWidget> PreviewViewport = FindPreviewViewport(Options.TargetWidget);
	return PreviewViewport.IsValid();
}

#ifdef WITH_TEXT_CHUNK_HELPER
void UPreviewViewportPrinter::RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options)
{
}

bool UPreviewViewportPrinter::CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const
{
	return false;
}
#endif

int32 UPreviewViewportPrinter::GetPriority() const
{
	return PreviewViewportPrinterPrinter;
}

FString UPreviewViewportPrinter::GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const
{
	// Since the object of the asset cannot be obtained from StandaloneAssetEditorToolkitHost etc.,
	// the name of the asset is obtained from the nearest graph editor.
	const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = GraphPrinter::FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(Widget);
	if (StandaloneAssetEditorToolkitHost.IsValid())
	{
		const TSharedPtr<SGraphEditorImpl> GraphEditor = GraphPrinter::FWidgetPrinterUtils::FindNearestChildGraphEditor(StandaloneAssetEditorToolkitHost);
		if (GraphEditor.IsValid())
		{
			if (const UEdGraph* Graph = GraphEditor->GetCurrentGraph())
			{
				if (const UObject* Outer = Graph->GetOuter())
				{
					return FString::Printf(TEXT("PreviewViewport-%s"), *Outer->GetName());
				}
			}
		}
	}
	
	return TEXT("PreviewViewport");
}

TSharedPtr<SWidget> UPreviewViewportPrinter::FindPreviewViewport(const TSharedPtr<SWidget>& TargetWidget) const
{
	TSharedPtr<SWidget> SearchTarget = TargetWidget;
	if (!SearchTarget.IsValid())
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		SearchTarget = GraphPrinter::FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
	}
	if (!SearchTarget.IsValid())
	{
		return nullptr;
	}

	TSharedPtr<SWidget> PreviewViewport = nullptr;
	GraphPrinter::FWidgetPrinterUtils::EnumerateChildWidgets(
		SearchTarget,
		[&](const TSharedPtr<SWidget> ChildWidget) -> bool
		{
			const TSharedPtr<SGlobalPlayWorldActions> GlobalPlayWorldActions = GP_CAST_SLATE_WIDGET(SGlobalPlayWorldActions, ChildWidget);
			if (GlobalPlayWorldActions.IsValid())
			{
				PreviewViewport = GlobalPlayWorldActions;
				return false;
			}

			return true;
		}
	);

	return PreviewViewport;
}

void UPreviewViewportPrinter::PrePrintPreviewViewport(
	EVisibility& PreviousVisibility,
	const TSharedPtr<SWidget>& PreviewViewport,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	const TSharedPtr<SOverlay> Overlay = GraphPrinter::FWidgetPrinterUtils::FindNearestChildOverlay(PreviewViewport);
	if (Overlay.IsValid())
	{
		PreviousVisibility = Overlay->GetVisibility();
		Overlay->SetVisibility(EVisibility::Collapsed);
	}
}

void UPreviewViewportPrinter::PostPrintPreviewViewport(
	const TSharedPtr<SWidget>& PreviewViewport,
	const EVisibility& PreviousVisibility,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	const TSharedPtr<SOverlay> Overlay = GraphPrinter::FWidgetPrinterUtils::FindNearestChildOverlay(PreviewViewport);
	if (Overlay.IsValid())
	{
		Overlay->SetVisibility(PreviousVisibility);
	}
}

#undef LOCTEXT_NAMESPACE
