// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWindow.h"
#include "SDetailsView.h"
#include "SDetailsViewBase.h"
#include "SActorDetails.h"
#include "SSubobjectInstanceEditor.h"

namespace GraphPrinter
{
	TSharedPtr<SDetailsView> FDetailsPanelPrinterUtils::FindNearestChildDetailsView(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SDetailsView> FoundDetailsView = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundDetailsView](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SDetailsView> DetailsView = GP_CAST_SLATE_WIDGET(SDetailsView, ChildWidget);
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

	TSharedPtr<SDetailsView> FDetailsPanelPrinterUtils::GetActiveDetailsView()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<SDetailsView> DetailsView = FindNearestChildDetailsView(DockingTabStack);
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

	TSharedPtr<SActorDetails> FDetailsPanelPrinterUtils::FindNearestChildActorDetailsView(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SActorDetails> FoundActorDetailsView = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundActorDetailsView](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SActorDetails> ActorDetailsView = GP_CAST_SLATE_WIDGET(SActorDetails, ChildWidget);
				if (ActorDetailsView.IsValid())
				{
					FoundActorDetailsView = ActorDetailsView;
					return false;
				}

				return true;
			}
		);

		return FoundActorDetailsView;
	}

	TSharedPtr<SActorDetails> FDetailsPanelPrinterUtils::GetActiveActorDetailsView()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<SActorDetails> ActorDetailsView = FindNearestChildActorDetailsView(DockingTabStack);
				if (ActorDetailsView.IsValid())
				{
					return ActorDetailsView;
				}
			}
		}

		const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (ActiveWindow.IsValid())
		{
			return FindNearestChildActorDetailsView(ActiveWindow);
		}

		return nullptr;
	}

	TSharedPtr<SWidget> FDetailsPanelPrinterUtils::FindNearestChildSubobjectInstanceEditor(TSharedPtr<SActorDetails> SearchTarget)
	{
		TSharedPtr<SWidget> FoundSubobjectInstanceEditor = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundSubobjectInstanceEditor](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SSubobjectInstanceEditor> SubobjectInstanceEditor = GP_CAST_SLATE_WIDGET(SSubobjectInstanceEditor, ChildWidget);
				if (SubobjectInstanceEditor.IsValid())
				{
					FoundSubobjectInstanceEditor = SubobjectInstanceEditor;
					return false;
				}

				return true;
			}
		);

		return FoundSubobjectInstanceEditor;
	}

	FVector2D FDetailsPanelPrinterUtils::GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(TSharedPtr<SWidget> Widget)
	{
		check(Widget.IsValid());
		
		const FVector2D DesiredSize = Widget->GetDesiredSize();
		
		const FGeometry& Geometry =
#if UE_4_24_OR_LATER
			Widget->GetTickSpaceGeometry();
#else
			Widget->GetCachedGeometry();
#endif
		const FVector2D LocalSize = Geometry.GetLocalSize();

		if (DesiredSize.ComponentwiseAllGreaterThan(LocalSize))
		{
			return (DesiredSize - LocalSize);
		}

		return FVector2D::ZeroVector;
	}
}
