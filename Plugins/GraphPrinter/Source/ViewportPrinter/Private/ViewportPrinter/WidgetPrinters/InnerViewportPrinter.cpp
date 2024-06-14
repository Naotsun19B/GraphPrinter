// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "ViewportPrinter/WidgetPrinters/InnerViewportPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "Widgets/Docking/SDockTab.h"

namespace GraphPrinter
{
	FViewportPrinter::FViewportPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FViewportPrinter::FViewportPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	TSharedPtr<SViewport> FViewportPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
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

	void FViewportPrinter::PreDrawWidget()
	{
		const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
		if (Overlay.IsValid())
		{
			ViewportPrinterParams.PreviousVisibility = Overlay->GetVisibility();
			Overlay->SetVisibility(EVisibility::Collapsed);
		}
	}

	void FViewportPrinter::PostDrawWidget()
	{
		const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
		if (Overlay.IsValid())
		{
			Overlay->SetVisibility(ViewportPrinterParams.PreviousVisibility);
		}
	}

	FString FViewportPrinter::GetWidgetTitle()
	{
		FString Title;
		GetViewportTitle(Widget, Title);
		return Title;
	}

	TSharedPtr<SViewport> FViewportPrinter::FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SViewport> FoundViewport = GP_CAST_SLATE_WIDGET(SViewport, SearchTarget);
		if (!FoundViewport.IsValid())
		{
			FWidgetPrinterUtils::EnumerateChildWidgets(
				SearchTarget,
				[&](const TSharedPtr<SWidget>& ChildWidget) -> bool
				{
					const TSharedPtr<SViewport> Viewport = GP_CAST_SLATE_WIDGET(SViewport, ChildWidget);
					if (Viewport.IsValid())
					{
						FoundViewport = Viewport;
						return false;
					}

					return true;
				}
			);
		}
		
		return FoundViewport;
	}

	bool FViewportPrinter::GetViewportTitle(const TSharedPtr<SViewport>& Viewport, FString& Title)
	{
		Title = TEXT("InvalidViewport");

		const TSharedPtr<SDockingTabStack> DockingTabStack = GP_CAST_SLATE_WIDGET(SDockingTabStack, FWidgetPrinterUtils::FindNearestParentDockingTabStack(Viewport));
		if (DockingTabStack.IsValid())
		{
			const TArray<TSharedRef<SDockTab>>& Tabs = DockingTabStack->GetTabs().AsArrayCopy();
			for (const auto& Tab : Tabs)
			{
				if (Tab->IsActive())
				{
					Title = Tab->GetTabLabel().ToString();
					return true;
				}
			}
				
			if (Tabs.Num() == 1)
			{
				Title = Tabs[0]->GetTabLabel().ToString();
				return true;
			}
		}
		
		return false;
	}
}
