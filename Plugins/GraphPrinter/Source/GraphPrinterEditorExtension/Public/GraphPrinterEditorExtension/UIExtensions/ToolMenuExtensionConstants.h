// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "GraphPrinterToolMenu"

namespace GraphPrinter
{
	/**
	 * Define here the common display name and tooltip text used by this plugin's editor extensions.
	 */
	namespace ToolMenuExtensionConstants
	{
		static FText ToolMenuLabel		= LOCTEXT("Label", "Graph Printer");
		static FText ToolMenuTooltip	= LOCTEXT("Tooltip", "You can perform the functions of Gprah Printer from here.");
	}
}

#undef LOCTEXT_NAMESPACE
