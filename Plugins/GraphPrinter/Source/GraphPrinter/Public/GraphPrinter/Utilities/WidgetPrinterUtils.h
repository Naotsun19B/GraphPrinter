// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

class SWidget;
class SDockTab;
class SDockingTabStack;
class SGraphEditorImpl;

namespace GraphPrinter
{
	/**
	 * Misc utility functions used in widget printer classes.
	 */
	class GRAPHPRINTER_API FWidgetPrinterUtils
	{
	public:
		// Get the extension by the format of the image file.
		static FString GetImageFileExtension(EDesiredImageFormat ImageFormat);

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
		static TSharedPtr<SDockingTabStack> FindNearestParentDockingTabStack(TSharedPtr<SDockTab> SearchTarget);
		
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
