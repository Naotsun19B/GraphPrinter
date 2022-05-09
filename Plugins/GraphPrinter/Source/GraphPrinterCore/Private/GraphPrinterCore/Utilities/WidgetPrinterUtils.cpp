// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/Utilities/WidgetPrinterUtils.h"
#include "GraphPrinterCore/Utilities/CastSlateWidget.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "SGraphEditorImpl.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace GraphPrinter
{
	namespace WidgetPrinterUtilsConstant
	{
		// The name of the minimap widget class.
		static const FName GraphMinimapClassName = TEXT("SGraphMinimap");
	}
	
	FString FWidgetPrinterUtils::GetImageFileExtension(const EDesiredImageFormat ImageFormat)
	{
#if BEFORE_UE_4_21
		if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDesiredImageFormat"), true))
		{
			const FString& Extension = EnumPtr->GetNameStringByIndex(static_cast<int32>(ImageFormat));
			return FString::Printf(TEXT(".%s"), *Extension.ToLower());
		}
#else
		if (const UEnum* EnumPtr = StaticEnum<EDesiredImageFormat>())
		{
			const FString& EnumString = EnumPtr->GetValueAsString(ImageFormat);
			FString UnusedString;
			FString Extension;
			if (EnumString.Split(TEXT("::"), &UnusedString, &Extension))
			{
				Extension = Extension.ToLower();
				return FString::Printf(TEXT(".%s"), *Extension);
			}
		}
#endif
	
		checkNoEntry();
		return {};
	}

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

	TSharedPtr<SDockingTabStack> FWidgetPrinterUtils::FindNearestParentDockingTabStack(TSharedPtr<SDockTab> SearchTarget)
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

	TSharedPtr<SGraphEditorImpl> FWidgetPrinterUtils::FindNearestChildGraphEditor(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SGraphEditorImpl> FoundGraphEditor = nullptr;
		
		EnumerateChildWidgets(
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

	TSharedPtr<SGraphEditorImpl> FWidgetPrinterUtils::GetActiveGraphEditor()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (ActiveTab.IsValid())
		{
			const TSharedPtr<SDockingTabStack> DockingTabStack = FindNearestParentDockingTabStack(ActiveTab);
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

	TSharedPtr<SWidget> FWidgetPrinterUtils::FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SWidget> FoundMinimap = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundMinimap](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SWidget> Minimap = Private::CastSlateWidget<SWidget>(
					ChildWidget,
					WidgetPrinterUtilsConstant::GraphMinimapClassName
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

	TArray<TSharedPtr<STextBlock>> FWidgetPrinterUtils::GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget)
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
