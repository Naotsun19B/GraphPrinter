// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class SOverlay;
class STextBlock;
class SGraphEditorImpl;
class SDockingTabStack;
class SStandaloneAssetEditorToolkitHost;

namespace GraphPrinter
{
	/**
	 * Misc utility functions used in widget printer classes.
	 */
	class WIDGETPRINTER_API FWidgetPrinterUtils
	{
	public:
		// Enumerate all widgets that are children of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateChildWidgets(
			TSharedPtr<SWidget> SearchTarget,
			TFunction<bool(const TSharedPtr<SWidget> ChildWidget)> Predicate
		);

		// Enumerate all widgets that are parents of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateParentWidgets(
			TSharedPtr<SWidget> SearchTarget,
			TFunction<bool(const TSharedPtr<SWidget> ParentWidget)> Predicate
		);

		// Find the docking tab stack that contains the specified dock tab.
		static TSharedPtr<SDockingTabStack> FindNearestParentDockingTabStack(TSharedPtr<SWidget> SearchTarget);
		
		// Returns the toolkit host of the Asset Editor that contains the specified tab.
		static TSharedPtr<SStandaloneAssetEditorToolkitHost> FindNearestParentStandaloneAssetEditorToolkitHost(TSharedPtr<SWidget> SearchTarget);
		
		// Find the widget that is the graph editor and nearest child of SearchTarget.
		static TSharedPtr<SGraphEditorImpl> FindNearestChildGraphEditor(TSharedPtr<SWidget> SearchTarget);

		// Returns the graph editor contained in the currently active tab.
		static TSharedPtr<SGraphEditorImpl> GetActiveGraphEditor();

		// Returns a minimap if it was a child of the graph editor.
		// What to do if GraphMinimap is installed.
		static TSharedPtr<SWidget> FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget);
		
		// Find the widget that is the overlay and nearest parent of SearchTarget.
		static TSharedPtr<SOverlay> FindNearestChildOverlay(TSharedPtr<SWidget> SearchTarget);

		// Returns the displayed text blocks that are children of SearchTarget.
		static TArray<TSharedPtr<STextBlock>> GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget);
	};
}
