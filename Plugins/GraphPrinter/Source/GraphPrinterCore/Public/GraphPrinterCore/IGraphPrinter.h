// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterCore/Types/PrintWidgetOptions.h"
#include "GraphPrinterCore/Types/RestoreWidgetOptions.h"

namespace GraphPrinter
{
	/**
	 * The public interface to the GraphPrinter module.
	 */
	class IGraphPrinter : public IModuleInterface
	{
	public:
		// The name of the module for this plugin.
		GRAPHPRINTERCORE_API static const FName PluginModuleName;
		
	public:
		// Returns singleton instance, loading the module on demand if needed.
		static IGraphPrinter& Get()
		{
			return FModuleManager::LoadModuleChecked<IGraphPrinter>(PluginModuleName);
		}

		// Returns whether the module is loaded and ready to use.
		static bool IsAvailable()
		{
			return FModuleManager::Get().IsModuleLoaded(PluginModuleName);
		}

		// Draw and export the widget with arguments.
		virtual void PrintWidget(const FPrintWidgetOptions& Options) = 0;
		
#ifdef WITH_TEXT_CHUNK_HELPER
		// Restore the state of the widget from the image file.
		virtual void RestoreWidget(const FRestoreWidgetOptions& Options = FRestoreWidgetOptions()) = 0;
#endif
	};
}