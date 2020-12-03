// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCore.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Slate/WidgetRenderer.h"
#include "GraphEditor.h"
#include "HAL/FileManager.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinterCoreDefine
{
	// Number of attempts to draw the widget on the render target.
	// The drawing result may be corrupted once.
	// Probably if draw twice, the drawing result will not be corrupted.
	static const int32 DrawTimes = 2;
}

const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Pending = SNotificationItem::ECompletionState::CS_Pending;
const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Success = SNotificationItem::ECompletionState::CS_Success;
const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Fail = SNotificationItem::ECompletionState::CS_Fail;

TSharedPtr<SNotificationItem> FGraphPrinterCore::ShowNotification(
	const FText& NotificationText, 
	TCompletionState CompletionState, 
	float ExpireDuration /* = 4.f */,
	ENotificationInteraction InteractionType /* = ENotificationInteraction::None */,
	const FText& InteractionText /* = FText()*/,
	FSimpleDelegate InteractionCallback /* = nullptr*/
)
{
	FNotificationInfo NotificationInfo(NotificationText);
	NotificationInfo.bFireAndForget = (ExpireDuration > 0.f);
	if (NotificationInfo.bFireAndForget)
	{
		NotificationInfo.ExpireDuration = ExpireDuration;
	}
	NotificationInfo.bUseLargeFont = true;
	//NotificationInfo.Image = &Brush;

	auto StateEnum = static_cast<SNotificationItem::ECompletionState>(CompletionState);
	switch (InteractionType)
	{
	case ENotificationInteraction::Hyperlink:
	{
		NotificationInfo.HyperlinkText = InteractionText;
		NotificationInfo.Hyperlink = InteractionCallback;
		break;
	}
	case ENotificationInteraction::Button:
	{
		FNotificationButtonInfo ButtonInfo(InteractionText, FText(), InteractionCallback, StateEnum);
		NotificationInfo.ButtonDetails.Add(ButtonInfo);
		break;
	}
	default: break;
	}

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetCompletionState(StateEnum);
	}
	
	return NotificationItem;
}

UWorld* FGraphPrinterCore::GetEditorWorld()
{
	TGuardValue<bool> UnattendedScriptGuard(GIsRunningUnattendedScript, true);

	if (!GIsEditor ||									// No editor instance.
		!IsInGameThread() ||							// Called by someone other than the game thread.
		(GEditor->PlayWorld || GIsPlayInEditorWorld))	// Running PIE.
	{
		return nullptr;
	}

	return GEditor->GetEditorWorldContext(false).World();
}

void FGraphPrinterCore::CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren)
{
	if (SearchTarget != nullptr)
	{
		if (FChildren* Children = SearchTarget->GetAllChildren())
		{
			for (int32 Index = 0; Index < Children->Num(); Index++)
			{
				TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
				OutChildren.Add(ChildWidget);
				CollectAllChildWidgets(ChildWidget, OutChildren);
			}
		}
	}
}

TSharedPtr<SGraphEditor> FGraphPrinterCore::FindGraphEditor(TSharedPtr<SWidget> SearchTarget)
{
	TArray<TSharedPtr<SWidget>> ChildWidgets;
	CollectAllChildWidgets(SearchTarget, ChildWidgets);

	for (const auto& ChildWidget : ChildWidgets)
	{
		TSharedPtr<SGraphEditor> GraphEditor = CAST_SLATE_WIDGET(SGraphEditor, ChildWidget);
		if (GraphEditor.IsValid())
		{
			return GraphEditor;
		}
	}

	return nullptr;
}

UTextureRenderTarget2D* FGraphPrinterCore::DrawWidgetToRenderTarget(
	TSharedPtr<SWidget> WidgetToRender,
	const FVector2D& DrawSize,
	bool bUseGamma,
	TextureFilter Filter
)
{
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGamma);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}
	if (bUseGamma)
	{
		RenderTarget->bForceLinearGamma = true;
		RenderTarget->UpdateResourceImmediate(true);
	}

	// Since the drawing result may be corrupted the first time, draw multiple times.
	for (int32 Count = 0; Count < GraphPrinterCoreDefine::DrawTimes; Count++)
	{
		WidgetRenderer->DrawWidget(RenderTarget, WidgetToRender.ToSharedRef(), DrawSize, 0.f, false);
		FlushRenderingCommands();
	}

	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

void FGraphPrinterCore::SaveTextureAsImageFile(UTexture* Texture, const FString& Filename, const FImageWriteOptions& Options)
{
	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
	{
		ShowNotification(ValidatePathErrorText, CS_Fail);
		return;
	}

	FString FullFilename = FPaths::ConvertRelativePathToFull(FPaths::GetBaseFilename(Filename, false));
	const FString& Extension = GetImageFileExtension(Options.Format);
	if (!Options.bOverwriteFile)
	{
		if (IFileManager::Get().FileExists(*FString(FullFilename + Extension)))
		{
			auto CombineFilenameAndIndex = [](const FString& Filename, int32 Index) -> FString
			{
				return FString::Printf(TEXT("%s_%d"), *Filename, Index);
			};

			int32 Index = 0;
			while (Index < INT32_MAX)
			{
				const FString& FilenameWithExtension = CombineFilenameAndIndex(FullFilename, Index) + Extension;
				if (!IFileManager::Get().FileExists(*FilenameWithExtension))
				{
					break;
				}
				Index++;
			}
			FullFilename = CombineFilenameAndIndex(FullFilename, Index);
		}
	}

	UImageWriteBlueprintLibrary::ExportToDisk(Texture, FullFilename + Extension, Options);
}

bool FGraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(
	FVector2D& DrawSize, 
	FVector2D& ViewLocation, 
	TSharedPtr<SGraphEditor> GraphEditor, 
	float Padding
)
{
	if (!GraphEditor.IsValid())
	{
		return false;
	}

	if (GraphEditor->GetNumberOfSelectedNodes() == 0)
	{
		return false;
	}

	FSlateRect Bounds;
	if (!GraphEditor->GetBoundsForSelectedNodes(Bounds, Padding + 100.f))
	{
		return false;
	}
	DrawSize = Bounds.GetSize();
	ViewLocation = Bounds.GetTopLeft();

	return true;
}

FString FGraphPrinterCore::GetGraphTitle(TSharedPtr<SGraphEditor> GraphEditor)
{
	if (GraphEditor.IsValid())
	{
		if (UEdGraph* Graph = GraphEditor->GetCurrentGraph())
		{
			// ReferenceViewer replaces outer name because Outer does not exist.
			if (Graph->IsA<UEdGraph_ReferenceViewer>())
			{
				return TEXT("ReferenceViewer");
			}
			else if (UObject* Outer = Graph->GetOuter())
			{
				return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			}
		}
	}

	return TEXT("Invalid GraphEditor");
}

FString FGraphPrinterCore::GetImageFileExtension(EDesiredImageFormat ImageFormat)
{
	if (UEnum* EnumPtr = StaticEnum<EDesiredImageFormat>())
	{
		const FString& EnumString = EnumPtr->GetValueAsString(ImageFormat);
		FString UnuseString;
		FString Extension;
		if (EnumString.Split(TEXT("::"), &UnuseString, &Extension))
		{
			Extension = Extension.ToLower();
			return FString::Printf(TEXT(".%s"), *Extension);
		}
	}

	return FString();
}

void FGraphPrinterCore::OpenFolderWithExplorer(const FString& FilePath)
{
	const FString& FullFilePath = FPaths::ConvertRelativePathToFull(FilePath);

	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(FullFilePath, &ValidatePathErrorText))
	{
		ShowNotification(ValidatePathErrorText, CS_Fail);
		return;
	}

	FPlatformProcess::ExploreFolder(*FullFilePath);
}

#undef LOCTEXT_NAMESPACE
