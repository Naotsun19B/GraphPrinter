// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class STextBlock;
class SGraphEditorImpl;

namespace GraphPrinter
{
	/**
	 * A utility class that defines utility functions used in generic graph printer classes.
	 */
	class GENERICGRAPHPRINTER_API FGenericGraphPrinterUtils
	{
	public:
		// Finds the widget that is the graph editor and nearest child of SearchTarget.
		static TSharedPtr<SGraphEditorImpl> FindNearestChildGraphEditor(const TSharedPtr<SWidget>& SearchTarget);

		// Returns the graph editor contained in the currently active tab.
		static TSharedPtr<SGraphEditorImpl> GetActiveGraphEditor();

		// Returns a minimap if it was a child of the graph editor.
		// What to do if GraphMinimap is installed.
		static TSharedPtr<SWidget> FindNearestChildMinimap(const TSharedPtr<SWidget>& SearchTarget);

		// Returns the displayed text blocks that are children of SearchTarget.
		static TArray<TSharedPtr<STextBlock>> GetVisibleChildTextBlocks(const TSharedPtr<SWidget>& SearchTarget);
	};
}
