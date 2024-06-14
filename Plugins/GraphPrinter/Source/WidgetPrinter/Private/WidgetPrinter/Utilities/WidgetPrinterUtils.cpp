// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "WidgetPrinter/ISupportedWidgetRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"
#include "Widgets/SOverlay.h"

namespace GraphPrinter
{
	void FWidgetPrinterUtils::EnumerateChildWidgets(
		const TSharedPtr<SWidget>& SearchTarget,
		const TFunction<bool(const TSharedPtr<SWidget>& ChildWidget)>& Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return;
		}

		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			if (ChildWidget.IsValid())
			{
				if (Predicate(ChildWidget))
				{
					EnumerateChildWidgets(ChildWidget, Predicate);
				}
			}
		}
	}

	void FWidgetPrinterUtils::EnumerateParentWidgets(
		const TSharedPtr<SWidget>& SearchTarget,
		const TFunction<bool(const TSharedPtr<SWidget>& ParentWidget)>& Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}
			
		const TSharedPtr<SWidget> ParentWidget = SearchTarget->GetParentWidget();
		if (!ParentWidget.IsValid())
		{
			return;
		}

		if (Predicate(ParentWidget))
		{
			EnumerateParentWidgets(ParentWidget, Predicate);
		}
	}

	TSharedPtr<SWidget> FWidgetPrinterUtils::FindNearestParentDockingTabStack(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SDockingTabStack> FoundDockingTabStack = GP_CAST_SLATE_WIDGET(SDockingTabStack, SearchTarget);
		if (!FoundDockingTabStack.IsValid())
		{
			EnumerateParentWidgets(
				SearchTarget,
				[&FoundDockingTabStack](const TSharedPtr<SWidget>& ParentWidget) -> bool
				{
					const TSharedPtr<SDockingTabStack> DockingTabStack = GP_CAST_SLATE_WIDGET(SDockingTabStack, ParentWidget);
					if (DockingTabStack.IsValid())
					{
						FoundDockingTabStack = DockingTabStack;
						return false;
					}

					return true;
				}
			);
		}

		return FoundDockingTabStack;
	}

	TSharedPtr<SWidget> FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SStandaloneAssetEditorToolkitHost> FoundStandaloneAssetEditorToolkitHost = GP_CAST_SLATE_WIDGET(SStandaloneAssetEditorToolkitHost, SearchTarget);
		if (!FoundStandaloneAssetEditorToolkitHost.IsValid())
		{
			EnumerateParentWidgets(
				SearchTarget,
				[&FoundStandaloneAssetEditorToolkitHost](const TSharedPtr<SWidget>& ParentWidget) -> bool
				{
					const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = GP_CAST_SLATE_WIDGET(SStandaloneAssetEditorToolkitHost, ParentWidget);
					if (StandaloneAssetEditorToolkitHost.IsValid())
					{
						FoundStandaloneAssetEditorToolkitHost = StandaloneAssetEditorToolkitHost;
						return false;
					}

					return true;
				}
			);
		}

		return FoundStandaloneAssetEditorToolkitHost;
	}
	
	TSharedPtr<SOverlay> FWidgetPrinterUtils::FindNearestChildOverlay(const TSharedPtr<SWidget>& SearchTarget)
	{
		TSharedPtr<SOverlay> FoundOverlay = GP_CAST_SLATE_WIDGET(SOverlay, SearchTarget);
		if (!FoundOverlay.IsValid())
		{
			EnumerateChildWidgets(
				SearchTarget,
				[&FoundOverlay](const TSharedPtr<SWidget>& ChildWidget) -> bool
				{
					const TSharedPtr<SOverlay> Overlay = GP_CAST_SLATE_WIDGET(SOverlay, ChildWidget);
					if (Overlay.IsValid())
					{
						FoundOverlay = Overlay;
						return false;
					}

					return true;
				}
			);
		}

		return FoundOverlay;
	}

	TSharedPtr<SWidget> FWidgetPrinterUtils::GetMostSuitableSearchTarget()
	{
		FSlateApplication& SlateApplication = FSlateApplication::Get();

		const auto& SupportedWidgetRegistry = ISupportedWidgetRegistry::Get();
		if (SupportedWidgetRegistry.WasAnyMenuVisibleInPreviousFrame())
		{
			const TOptional<FSupportedWidget>& SelectedWidget = SupportedWidgetRegistry.GetSelectedWidget();
			if (SelectedWidget.IsSet() && SelectedWidget->IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedWidget->GetWidget()->GetTypeAsString());
				return SelectedWidget->GetWidget();
			}
		}
		
		const FWidgetPath WidgetsUnderCursor = SlateApplication.LocateWindowUnderMouse(
			SlateApplication.GetCursorPos(), SlateApplication.GetInteractiveTopLevelWindows()
		);
		if (WidgetsUnderCursor.IsValid())
		{
			const FArrangedChildren& Widgets = WidgetsUnderCursor.Widgets;
			return Widgets.Last().Widget;
		}

		return nullptr;
	}
}
