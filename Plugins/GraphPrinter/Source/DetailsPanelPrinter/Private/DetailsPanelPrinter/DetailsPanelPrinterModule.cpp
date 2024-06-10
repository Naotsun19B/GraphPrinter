// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace GraphPrinter
{
	class FDetailsPanelPrinterModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FDetailsPanelPrinterModule::StartupModule()
	{
	}

	void FDetailsPanelPrinterModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinter::FDetailsPanelPrinterModule, DetailsPanelPrinter)
