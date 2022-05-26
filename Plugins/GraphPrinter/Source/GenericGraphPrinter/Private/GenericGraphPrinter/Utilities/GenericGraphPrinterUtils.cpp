// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Utilities/GenericGraphPrinterUtils.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Application/SlateApplication.h"
#include "SGraphEditorImpl.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Docking/SDockTab.h"

namespace GraphPrinter
{
	namespace GenericGraphPrinterUtilsConstant
	{
		// The name of the minimap widget class.
		static const FName GraphMinimapClassName = TEXT("SGraphMinimap");
	}
	
	TSharedPtr<SGraphEditorImpl> FGenericGraphPrinterUtils::FindNearestChildGraphEditor(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SGraphEditorImpl> FoundGraphEditor = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundGraphEditor](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, ChildWidget);
				if (GraphEditor.IsValid())
				{
					FoundGraphEditor = GraphEditor;
					return false;
				}

				return true;
			}
		);

		return FoundGraphEditor;
	}

	TSharedPtr<SGraphEditorImpl> FGenericGraphPrinterUtils::GetActiveGraphEditor()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(ActiveTab);
			if (DockingTabStack.IsValid())
			{
				TSharedPtr<SGraphEditorImpl> GraphEditor = FindNearestChildGraphEditor(DockingTabStack);
				if (GraphEditor.IsValid())
				{
					return GraphEditor;
				}
			}
		}

		const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (ActiveWindow.IsValid())
		{
			return FindNearestChildGraphEditor(ActiveWindow);
		}

		return nullptr;
	}

	TSharedPtr<SWidget> FGenericGraphPrinterUtils::FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SWidget> FoundMinimap = nullptr;
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			SearchTarget,
			[&FoundMinimap](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SWidget> Minimap = Private::CastSlateWidget<SWidget>(
					ChildWidget,
					GenericGraphPrinterUtilsConstant::GraphMinimapClassName
				);
				if (Minimap.IsValid())
				{
					FoundMinimap = Minimap;
					return false;
				}

				return true;
			}
		);

		return FoundMinimap;
	}

	TArray<TSharedPtr<STextBlock>> FGenericGraphPrinterUtils::GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget)
	{
		if (!SearchTarget.IsValid())
		{
			return {};
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return {};
		}

		TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks;
		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			const TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			TSharedPtr<STextBlock> TextBlock = GP_CAST_SLATE_WIDGET(STextBlock, ChildWidget);
			if (TextBlock.IsValid())
			{
				if (TextBlock->GetVisibility().IsVisible())
				{
					VisibleChildTextBlocks.Add(TextBlock);
				}
			}
		}
		
		return VisibleChildTextBlocks;
	}
}
