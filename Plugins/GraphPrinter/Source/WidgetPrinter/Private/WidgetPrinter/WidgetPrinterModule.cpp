// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "WidgetPrinter/ISupportedWidgetHolder.h"

namespace GraphPrinter
{
	class FWidgetPrinterModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FWidgetPrinterModule::StartupModule()
	{
		// Registers widget printer registry.
		IWidgetPrinterRegistry::Register();

		// Registers supported widget holder.
		ISupportedWidgetHolder::Register();
	}

	void FWidgetPrinterModule::ShutdownModule()
	{
		// Unregisters supported widget holder.
		ISupportedWidgetHolder::Unregister();
		
		// Unregisters widget printer registry.
		IWidgetPrinterRegistry::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FWidgetPrinterModule, WidgetPrinter)
