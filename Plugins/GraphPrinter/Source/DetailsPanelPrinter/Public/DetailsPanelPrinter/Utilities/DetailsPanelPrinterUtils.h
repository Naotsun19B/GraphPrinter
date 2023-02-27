// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IDetailsView;
class SWidget;

namespace GraphPrinter
{
	/**
	 * Misc utility functions used in details panel printer classes.
	 */
	class DETAILSPANELPRINTER_API FDetailsPanelPrinterUtils
	{
	public:
		// Find the widget that is the graph editor and nearest child of SearchTarget.
		static TSharedPtr<IDetailsView> FindNearestChildDetailsView(TSharedPtr<SWidget> SearchTarget);

		// Returns the details view contained in the currently active tab.
		static TSharedPtr<IDetailsView> GetActiveDetailsView();
	};
}
