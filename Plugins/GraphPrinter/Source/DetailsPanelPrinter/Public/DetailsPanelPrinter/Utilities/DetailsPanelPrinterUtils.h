// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class SDetailsView;
class SActorDetails;
class FJsonObject;

namespace GraphPrinter
{
	/**
	 *  A utility class that defines utility functions used in details panel printer classes.
	 */
	class DETAILSPANELPRINTER_API FDetailsPanelPrinterUtils
	{
	public:
		// Finds the widget that is the details view and nearest child of SearchTarget.
		static TSharedPtr<SDetailsView> FindNearestChildDetailsView(const TSharedPtr<SWidget>& SearchTarget);

		// Returns the details view contained in the currently active tab.
		static TSharedPtr<SDetailsView> GetActiveDetailsView();

		// Finds the widget that is the actor details view and nearest child of SearchTarget.
		static TSharedPtr<SActorDetails> FindNearestChildActorDetailsView(const TSharedPtr<SWidget>& SearchTarget);

		// Returns the actor details view contained in the currently active tab.
		static TSharedPtr<SActorDetails> GetActiveActorDetailsView();

		// Finds the widget that is the subobject instance editor and nearest child of SearchTarget.
		static TSharedPtr<SWidget> FindNearestChildSubobjectInstanceEditor(const TSharedPtr<SActorDetails>& SearchTarget);

		// Returns the difference between the widget's actual visible size and its overall size.
		static FVector2D GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(TSharedPtr<SWidget> Widget);
		
		// Imports properties of UObject from Json formatted string.
		static bool ImportObjectPropertiesFromJsonString(UObject* Object, const FString& JsonString);

		// Exports properties of UObject as Json formatted string.
		static bool ExportObjectPropertiesAsJsonString(UObject* Object, FString& JsonString);

		// Writes all properties recorded in the json object to the UObject and returns the number of properties read.
		static int32 JsonObjectToUObject(UObject& Object, const TSharedRef<FJsonObject>& JsonObject);
		
		// Writes all properties that match the specified flags of the UObject to the json object and returns the number of properties written.
		static int32 UObjectToJsonObject(
			UObject& Object,
			const TSharedRef<FJsonObject>& JsonObject,
			const int64 CheckFlags = 0,
			const int64 SkipFlags = 0
		);
	};
}
