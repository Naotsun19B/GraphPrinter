// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"
#include "Widgets/SOverlay.h"

namespace GraphPrinter
{
	void FWidgetPrinterUtils::EnumerateChildWidgets(
		TSharedPtr<SWidget> SearchTarget,
		TFunction<bool(const TSharedPtr<SWidget> ChildWidget)> Predicate
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
		TSharedPtr<SWidget> SearchTarget,
		TFunction<bool(const TSharedPtr<SWidget> ParentWidget)> Predicate
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

	TSharedPtr<SWidget> FWidgetPrinterUtils::FindNearestParentDockingTabStack(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SDockingTabStack> FoundDockingTabStack = nullptr;
		
		EnumerateParentWidgets(
			SearchTarget,
			[&FoundDockingTabStack](const TSharedPtr<SWidget> ParentWidget) -> bool
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

		return FoundDockingTabStack;
	}

	TSharedPtr<SStandaloneAssetEditorToolkitHost> FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SStandaloneAssetEditorToolkitHost> FoundStandaloneAssetEditorToolkitHost = nullptr;
		
		EnumerateParentWidgets(
			SearchTarget,
			[&FoundStandaloneAssetEditorToolkitHost](const TSharedPtr<SWidget> ParentWidget) -> bool
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

		return FoundStandaloneAssetEditorToolkitHost;
	}
	
	TSharedPtr<SOverlay> FWidgetPrinterUtils::FindNearestChildOverlay(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SOverlay> FoundOverlay = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundOverlay](const TSharedPtr<SWidget> ChildWidget) -> bool
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

		return FoundOverlay;
	}
}
