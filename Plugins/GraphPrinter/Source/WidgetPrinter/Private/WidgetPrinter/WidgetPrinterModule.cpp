// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"

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
		// Register widget printer registry.
		IWidgetPrinterRegistry::Register();
	}

	void FWidgetPrinterModule::ShutdownModule()
	{
		// Unregister widget printer registry.
		IWidgetPrinterRegistry::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FWidgetPrinterModule, WidgetPrinter)
