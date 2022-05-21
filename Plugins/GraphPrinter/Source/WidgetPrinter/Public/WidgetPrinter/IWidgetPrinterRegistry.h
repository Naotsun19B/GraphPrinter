// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"

namespace GraphPrinter
{
	/**
	 * A class that allows you to collect and search widget printers.
	 */
	class WIDGETPRINTER_API IWidgetPrinterRegistry
	{
	public:
		// Register-Unregister and instance getter this class.
		static void Register();
		static void Unregister();
		static IWidgetPrinterRegistry& Get();
		
		// Destructor.
		virtual ~IWidgetPrinterRegistry() = default;
		
		// Draw and export the widget with arguments.
		virtual void PrintWidget(UPrintWidgetOptions* Options) = 0;

		// Returns whether the command to print the widget can be executed.
		virtual bool CanPrintWidget(UPrintWidgetOptions* Options) = 0;
		
		// Restore the state of the widget from the image file.
		virtual void RestoreWidget(URestoreWidgetOptions* Options) = 0;

		// Returns whether the command to restore the widget can be executed.
		virtual bool CanRestoreWidget(URestoreWidgetOptions* Options) = 0;
	};
}
