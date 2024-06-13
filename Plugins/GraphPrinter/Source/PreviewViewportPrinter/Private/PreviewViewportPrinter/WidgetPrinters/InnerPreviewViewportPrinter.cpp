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

		return FindTargetWidgetFromSearchTarget(ActualSearchTarget);
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
		FString Title;
		GetPreviewViewportTitle(Widget, Title);
		return Title;
	}

	TSharedPtr<SGlobalPlayWorldActions> FPreviewViewportPrinter::FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SGlobalPlayWorldActions> PreviewViewport = nullptr;
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
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

	bool FPreviewViewportPrinter::GetPreviewViewportTitle(const TSharedPtr<SGlobalPlayWorldActions>& PreviewViewport, FString& Title)
	{
		Title = TEXT("PreviewViewport");
		
		// #TODO: Need to find a way that doesn't go through the graph editor.
		// Since the object of the asset cannot be obtained from StandaloneAssetEditorToolkitHost etc., the name of the asset is obtained from the nearest graph editor.
		const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(PreviewViewport);
		if (!StandaloneAssetEditorToolkitHost.IsValid())
		{
			return false;
		}

		const TSharedPtr<SGraphEditorImpl> GraphEditor = FGenericGraphPrinterUtils::FindNearestChildGraphEditor(StandaloneAssetEditorToolkitHost);
		if (!GraphEditor.IsValid())
		{
			return false;
		}

		const UEdGraph* Graph = GraphEditor->GetCurrentGraph();
		if (!IsValid(Graph))
		{
			return false;
		}

		const UObject* Outer = Graph->GetOuter();
		if (!IsValid(Outer))
		{
			return false;
		}
		
		Title = FString::Printf(TEXT("PreviewViewport-%s"), *Outer->GetName());
		return true;
	}
}
