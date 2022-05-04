// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"

namespace GraphPrinterCore
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
		// Register settings.
		UGraphPrinterSettings::Register();
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinterCore::FGraphPrinterCoreModule, GraphPrinter)
