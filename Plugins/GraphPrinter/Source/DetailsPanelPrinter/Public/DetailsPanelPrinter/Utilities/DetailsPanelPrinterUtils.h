// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class SDetailsView;
class SActorDetails;

namespace GraphPrinter
{
	/**
	 * Misc utility functions used in details panel printer classes.
	 */
	class DETAILSPANELPRINTER_API FDetailsPanelPrinterUtils
	{
	public:
		// Finds the widget that is the details view and nearest child of SearchTarget.
		static TSharedPtr<SDetailsView> FindNearestChildDetailsView(TSharedPtr<SWidget> SearchTarget);

		// Returns the details view contained in the currently active tab.
		static TSharedPtr<SDetailsView> GetActiveDetailsView();

		// Finds the widget that is the actor details view and nearest child of SearchTarget.
		static TSharedPtr<SActorDetails> FindNearestChildActorDetailsView(TSharedPtr<SWidget> SearchTarget);

		// Returns the actor details view contained in the currently active tab.
		static TSharedPtr<SActorDetails> GetActiveActorDetailsView();

		// Finds the widget that is the subobject instance editor and nearest child of SearchTarget.
		static TSharedPtr<SWidget> FindNearestChildSubobjectInstanceEditor(TSharedPtr<SActorDetails> SearchTarget);

		// Returns the difference between the widget's actual visible size and its overall size.
		static FVector2D GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(TSharedPtr<SWidget> Widget);
	};
}
