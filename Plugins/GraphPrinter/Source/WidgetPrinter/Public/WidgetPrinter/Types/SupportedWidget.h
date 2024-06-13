// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class UWidgetPrinter;

namespace GraphPrinter
{
	/**
	 * A struct of information about widgets supported by one of the printers.
	 */
	struct WIDGETPRINTER_API FSupportedWidget
	{
	public:
		// Constructor.
		FSupportedWidget(const TSharedRef<SWidget>& InWidget, const FString& InDisplayName, const int32 InPriority);

		// Returns whether the widget that this data has is enabled.
		bool IsValid() const;

		// Returns the unique ID to identify data.
		const FGuid& GetIdentifier() const;
		
		// Returns the supported widget instance.
		TSharedPtr<SWidget> GetWidget() const;

		// Returns the name displayed on the editor UI.
		const FText& GetDisplayName() const;

		// Overload operators for use with arrays.
		bool operator==(const FSupportedWidget& Other) const;
		bool operator<(const FSupportedWidget& Other) const;

	private:
		// The unique ID to identify data.
		FGuid Identifier;
		
		// The supported widget instance.
		TWeakPtr<SWidget> Widget;

		// The name displayed on the editor UI.
		FText DisplayName;

		// The priority of the corresponding printer.
		int32 Priority;
	};
}
