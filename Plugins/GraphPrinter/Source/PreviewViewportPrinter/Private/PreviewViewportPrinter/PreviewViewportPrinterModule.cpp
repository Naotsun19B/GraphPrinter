// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace GraphPrinter
{
	class FPreviewViewportPrinterModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FPreviewViewportPrinterModule::StartupModule()
	{
	}

	void FPreviewViewportPrinterModule::ShutdownModule()
	{
	}
}

IMPLEMENT_MODULE(GraphPrinter::FPreviewViewportPrinterModule, PreviewViewportPrinter)
