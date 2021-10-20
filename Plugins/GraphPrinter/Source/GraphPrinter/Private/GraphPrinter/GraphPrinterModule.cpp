// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinter/GraphPrinterGlobals.h"
#include "GraphPrinter/GraphPrinterCommands.h"
#include "GraphPrinter/GraphPrinterSettings.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinter
{
	class FGraphPrinterModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphPrinterModule::StartupModule()
	{
		UGraphPrinterSettings::Register();
		FGraphPrinterCommands::Register();
		FGraphPrinterCommands::Bind();
	}

	void FGraphPrinterModule::ShutdownModule()
	{
		FGraphPrinterCommands::Unregister();
		UGraphPrinterSettings::Unregister();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterModule, GraphPrinter)
