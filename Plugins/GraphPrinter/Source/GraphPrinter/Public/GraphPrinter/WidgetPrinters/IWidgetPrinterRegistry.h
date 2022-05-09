// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/WidgetPrinters/WidgetPrinter.h"

namespace GraphPrinter
{
	/**
	 * A class that allows you to collect and search widget printers.
	 */
	class GRAPHPRINTER_API IWidgetPrinterRegistry
	{
	public:
		// Register-Unregister and instance getter this class.
		static void Register();
		static void Unregister();
		static IWidgetPrinterRegistry& Get();
		
		// Destructor.
		virtual ~IWidgetPrinterRegistry() = default;
		
		// Returns a widget printer that meets the criteria.
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(const FPrintWidgetOptions& Options) const = 0;
#ifdef WITH_TEXT_CHUNK_HELPER
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(const FRestoreWidgetOptions& Options) const = 0;
#endif
	};
}
