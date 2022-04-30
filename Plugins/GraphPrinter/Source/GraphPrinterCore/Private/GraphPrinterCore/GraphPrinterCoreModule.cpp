// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace GraphPrinter
{
	class FGraphPrinterCoreModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphPrinterCoreModule::StartupModule()
	{
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterCoreModule, GraphPrinter)
