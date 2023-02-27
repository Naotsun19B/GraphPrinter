// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"

namespace GraphPrinter
{
	namespace DetailsPanelPrinterUtilsConstant
	{
		// The name of the details view widget class.
		static const FName DetailsViewClassName = TEXT("SDetailsView");
	}
	
	TSharedPtr<IDetailsView> FDetailsPanelPrinterUtils::FindNearestChildDetailsView(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<IDetailsView> FoundDetailsView = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundDetailsView](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<IDetailsView> DetailsView = Private::CastSlateWidget<IDetailsView, SWidget>(ChildWidget, DetailsPanelPrinterUtilsConstant::DetailsViewClassName);
				if (DetailsView.IsValid())
				{
					FoundDetailsView = DetailsView;
					return false;
				}

				return true;
			}
		);

		return FoundDetailsView;
	}

	TSharedPtr<IDetailsView> FDetailsPanelPrinterUtils::GetActiveDetailsView()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<IDetailsView> DetailsView = FindNearestChildDetailsView(DockingTabStack);
				if (DetailsView.IsValid())
				{
					return DetailsView;
				}
			}
		}

		const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (ActiveWindow.IsValid())
		{
			return FindNearestChildDetailsView(ActiveWindow);
		}

		return nullptr;
	}
}
