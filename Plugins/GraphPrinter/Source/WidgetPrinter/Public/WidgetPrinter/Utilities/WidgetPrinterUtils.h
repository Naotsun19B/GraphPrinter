// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class SWindow;
class SOverlay;

namespace GraphPrinter
{
	/**
	 * A utility class that defines utility functions used in widget printer classes.
	 */
	class WIDGETPRINTER_API FWidgetPrinterUtils
	{
	public:
		// Enumerates all widgets that are children of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateChildWidgets(
			const TSharedPtr<SWidget>& SearchTarget,
			const TFunction<bool(const TSharedPtr<SWidget>& ChildWidget)>& Predicate
		);

		// Enumerates all widgets that are parents of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateParentWidgets(
			const TSharedPtr<SWidget>& SearchTarget,
			const TFunction<bool(const TSharedPtr<SWidget>& ParentWidget)>& Predicate
		);

		// Finds the docking tab stack that contains the specified dock tab.
		static TSharedPtr<SWidget> FindNearestParentDockingTabStack(const TSharedPtr<SWidget>& SearchTarget);
		
		// Returns the toolkit host of the Asset Editor that contains the specified tab.
		static TSharedPtr<SWidget> FindNearestParentStandaloneAssetEditorToolkitHost(const TSharedPtr<SWidget>& SearchTarget);
		
		// Finds the widget that is the overlay and nearest parent of SearchTarget.
		static TSharedPtr<SOverlay> FindNearestChildOverlay(const TSharedPtr<SWidget>& SearchTarget);
		
		// Returns the nearest docking tab stack from the widget under the mouse cursor.
		// Returns nullptr if the widget under the mouse cursor belongs to a menu stack window.
		static TSharedPtr<SWidget> GetMostSuitableSearchTarget();
	};
}
