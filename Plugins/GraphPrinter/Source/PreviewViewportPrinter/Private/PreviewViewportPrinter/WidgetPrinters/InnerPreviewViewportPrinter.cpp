// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "PreviewViewportPrinter/WidgetPrinters/InnerPreviewViewportPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterUtils.h"
#include "SGraphEditorImpl.h"
#include "Widgets/Docking/SDockTab.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"

namespace GraphPrinter
{
	FPreviewViewportPrinter::FPreviewViewportPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FPreviewViewportPrinter::FPreviewViewportPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	TSharedPtr<SGlobalPlayWorldActions> FPreviewViewportPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
	{
		TSharedPtr<SWidget> ActualSearchTarget = SearchTarget;
		if (!ActualSearchTarget.IsValid())
		{
			const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
			const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
			ActualSearchTarget = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
		}
		if (!ActualSearchTarget.IsValid())
		{
			return nullptr;
		}

		TSharedPtr<SGlobalPlayWorldActions> PreviewViewport = nullptr;
		FWidgetPrinterUtils::EnumerateChildWidgets(
			ActualSearchTarget,
			[&](const TSharedPtr<SWidget>& ChildWidget) -> bool
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

	void FPreviewViewportPrinter::PreDrawWidget()
	{
		const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
		if (Overlay.IsValid())
		{
			PreviewViewportPrinterParams.PreviousVisibility = Overlay->GetVisibility();
			Overlay->SetVisibility(EVisibility::Collapsed);
		}
	}

	void FPreviewViewportPrinter::PostDrawWidget()
	{
		const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
		if (Overlay.IsValid())
		{
			Overlay->SetVisibility(PreviewViewportPrinterParams.PreviousVisibility);
		}
	}

	FString FPreviewViewportPrinter::GetWidgetTitle()
	{
		// #TODO: Need to find a way that doesn't go through the graph editor.
		// Since the object of the asset cannot be obtained from StandaloneAssetEditorToolkitHost etc., the name of the asset is obtained from the nearest graph editor.
		const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(Widget);
		if (StandaloneAssetEditorToolkitHost.IsValid())
		{
			const TSharedPtr<SGraphEditorImpl> GraphEditor = FGenericGraphPrinterUtils::FindNearestChildGraphEditor(StandaloneAssetEditorToolkitHost);
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
}
