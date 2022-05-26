// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class STextBlock;
class SGraphEditorImpl;

namespace GraphPrinter
{
	/**
	 * Misc utility functions used in generic graph printer classes.
	 */
	class GENERICGRAPHPRINTER_API FGenericGraphPrinterUtils
	{
	public:
		// Find the widget that is the graph editor and nearest child of SearchTarget.
		static TSharedPtr<SGraphEditorImpl> FindNearestChildGraphEditor(TSharedPtr<SWidget> SearchTarget);

		// Returns the graph editor contained in the currently active tab.
		static TSharedPtr<SGraphEditorImpl> GetActiveGraphEditor();

		// Returns a minimap if it was a child of the graph editor.
		// What to do if GraphMinimap is installed.
		static TSharedPtr<SWidget> FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget);

		// Returns the displayed text blocks that are children of SearchTarget.
		static TArray<TSharedPtr<STextBlock>> GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget);
	};
}
