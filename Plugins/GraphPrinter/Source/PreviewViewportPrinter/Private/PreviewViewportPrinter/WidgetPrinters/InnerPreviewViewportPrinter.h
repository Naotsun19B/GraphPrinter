// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterUtils.h"
#include "Kismet2/DebuggerCommands.h"
#include "SGraphEditorImpl.h"
#include "Widgets/Docking/SDockTab.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class PREVIEWVIEWPORTPRINTER_API FPreviewViewportPrinter
		: public TInnerWidgetPrinter<SGlobalPlayWorldActions, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerWidgetPrinter<SGlobalPlayWorldActions, UPrintWidgetOptions, URestoreWidgetOptions>;
	
	public:
		// Constructor.
		FPreviewViewportPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FPreviewViewportPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}
		
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SGlobalPlayWorldActions> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
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
		virtual void PreDrawWidget() override
		{
			const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
			if (Overlay.IsValid())
			{
				PreviewViewportPrinterParams.PreviousVisibility = Overlay->GetVisibility();
				Overlay->SetVisibility(EVisibility::Collapsed);
			}
		}
		virtual void PostDrawWidget() override
		{
			const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
			if (Overlay.IsValid())
			{
				Overlay->SetVisibility(PreviewViewportPrinterParams.PreviousVisibility);
			}
		}
		virtual FString GetWidgetTitle() override
		{
			// #TODO: Need to find a way that doesn't go through the graph editor.
			// Since the object of the asset cannot be obtained from StandaloneAssetEditorToolkitHost etc.,
			// the name of the asset is obtained from the nearest graph editor.
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
		// End of TInnerWidgetPrinter interface.

	protected:
		// A group of parameters that must be retained for processing.
		struct FPreviewViewportPrinterParams
		{
			// Keep the original visibility to temporarily hide the viewport menu widget.
			EVisibility PreviousVisibility;
		};
		FPreviewViewportPrinterParams PreviewViewportPrinterParams;
	};
}
