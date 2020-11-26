// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterCommands.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

class FGraphPrinterModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.
};

#define LOCTEXT_NAMESPACE "FGraphPrinterModule"

void FGraphPrinterModule::StartupModule()
{
	GraphPrinterCommands::Register();
	GraphPrinterCommands::Bind();
}

void FGraphPrinterModule::ShutdownModule()
{
	GraphPrinterCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGraphPrinterModule, GraphPrinter)
