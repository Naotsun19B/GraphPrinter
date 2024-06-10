// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphPrinter
{
	/**
	 * A class that holds the widgets that are currently available and supported by any printer.
	 */
	class WIDGETPRINTER_API ISupportedWidgetRegistry
	{
	public:
		// Registers-Unregisters and instance getter this class.
		static void Register();
		static void Unregister();
		static ISupportedWidgetRegistry& Get();

		// Destructor.
		virtual ~ISupportedWidgetRegistry() = default;

		// Returns a list of widgets currently available and supported by any printer.
		virtual TArray<TSharedRef<SWidget>> GetSupportedWidgets() const = 0;

		// Returns the widget selected from the menu.
		virtual TSharedPtr<SWidget> GetSelectedWidget() const = 0;

		// Selects the widget specified by index from the supported widgets.
		virtual void SetSelectedWidget(const int32 Index) = 0;
	};
}
