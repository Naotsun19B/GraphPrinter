// Copyright 2020-2024 Naotsun. All Rights Reserved.

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
		// Registers remote control receiver.
		FGraphPrinterRemoteControlReceiver::Register();
	}

	void FGraphPrinterRemoteControlModule::ShutdownModule()
	{
		// Unregisters remote control receiver.
		FGraphPrinterRemoteControlReceiver::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterRemoteControlModule, GraphPrinterRemoteControl)
