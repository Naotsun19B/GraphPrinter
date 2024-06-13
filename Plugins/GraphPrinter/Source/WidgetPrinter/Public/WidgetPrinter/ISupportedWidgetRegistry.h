// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/Types/SupportedWidget.h"

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

		// Collects supported widgets from the currently displayed screen.
		virtual void CollectSupportedWidget() = 0;

		// Returns a list of widgets currently available and supported by any printer.
		virtual const TArray<FSupportedWidget>& GetSupportedWidgets() const = 0;

		// Returns the widget selected from the menu.
		virtual TOptional<FSupportedWidget> GetSelectedWidget() const = 0;

		// Selects the widget specified by index from the supported widgets.
		virtual void SetSelectedWidget(const FGuid Identifier) = 0;

		// Returns whether menus such as the tools menu and toolbar were visible in the previous frame.
		virtual bool WasAnyMenuVisibleInPreviousFrame() const = 0;
	};
}
