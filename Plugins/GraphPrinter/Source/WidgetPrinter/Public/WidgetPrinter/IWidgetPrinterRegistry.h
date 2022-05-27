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
		
		// Returns a widget printer that meets the criteria.
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(UPrintWidgetOptions*  Options) const = 0;
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(URestoreWidgetOptions* Options) const = 0;
	};
}
