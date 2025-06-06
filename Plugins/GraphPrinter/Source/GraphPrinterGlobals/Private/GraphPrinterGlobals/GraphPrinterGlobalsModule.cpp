// Copyright 2020-2025 Naotsun. All Rights Reserved.

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
		// Registers settings.
		UGraphPrinterSettings::Register();
	}

	void FGraphPrinterGlobalsModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterGlobalsModule, GraphPrinterGlobals)
