// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

namespace GraphPrinter
{
	class FGraphPrinterGlobalsModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphPrinterGlobalsModule::StartupModule()
	{
		// Register settings.
		UGraphPrinterSettings::Register();
	}

	void FGraphPrinterGlobalsModule::ShutdownModule()
	{
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterGlobalsModule, GraphPrinterGlobals)
