// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace GraphPrinter
{
	class FViewportPrinterModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FViewportPrinterModule::StartupModule()
	{
	}

	void FViewportPrinterModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinter::FViewportPrinterModule, ViewportPrinter)
