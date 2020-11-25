// Copyright Epic Games, Inc. All Rights Reserved.

#include "GraphPrinter.h"
#include "GraphPrinterCommands.h"
#include "GraphPrinterUtils.h"
#include "Interfaces/IMainFrameModule.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

#define LOCTEXT_NAMESPACE "FGraphPrinterModule"

void FGraphPrinterModule::StartupModule()
{
	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();

	GraphPrinterCommands::Register();

	CommandBindings->MapAction(
		GraphPrinterCommands::Get().PrintGraph,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraph)
	);
}

void FGraphPrinterModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGraphPrinterModule, GraphPrinter)
