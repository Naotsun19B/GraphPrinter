// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

namespace GraphPrinterGlobals
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
	}

	void FGraphPrinterGlobalsModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinterGlobals::FGraphPrinterGlobalsModule, GraphPrinterGlobals)
