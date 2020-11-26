// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCommands.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterUtils.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrintCommands"

GraphPrinterCommands::GraphPrinterCommands()
	: TCommands<GraphPrinterCommands>
	(
		TEXT("GraphPrinter"),
		NSLOCTEXT("Contexts", "GraphPrinter", "Graph Printer"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
{
}

void GraphPrinterCommands::RegisterCommands()
{
	UI_COMMAND(PrintGraph, "Print Graph", "Exports the currently active graph editor as an image.", EUserInterfaceActionType::None, FInputChord(EKeys::F9, EModifierKey::Control));
}

void GraphPrinterCommands::Bind()
{
	if (!GraphPrinterCommands::IsRegistered())
	{
		UE_LOG(LogGraphPrinter, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
	}

	GraphPrinterCommands::Get().BindCommands();
}

void GraphPrinterCommands::BindCommands() const
{
	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();

	CommandBindings->MapAction(
		GraphPrinterCommands::Get().PrintGraph,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraph)
	);
}

#undef LOCTEXT_NAMESPACE