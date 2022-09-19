// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterRemoteControl/WebSockets/GraphPrinterRemoteControlReceiver.h"

namespace GraphPrinter
{
	class FGraphPrinterRemoteControlModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphPrinterRemoteControlModule::StartupModule()
	{
		// Register remote control receiver.
		FGraphPrinterRemoteControlReceiver::Register();
	}

	void FGraphPrinterRemoteControlModule::ShutdownModule()
	{
		// Unregister remote control receiver.
		FGraphPrinterRemoteControlReceiver::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterRemoteControlModule, GraphPrinterRemoteControl)
