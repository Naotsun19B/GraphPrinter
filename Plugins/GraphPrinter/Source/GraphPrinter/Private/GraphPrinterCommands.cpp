// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCommands.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterUtils.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrintCommands"

#define REGISTER_COMMAND(CommandName, InDescription, InDefaultChord) \
	UI_COMMAND(CommandName, #CommandName, InDescription, EUserInterfaceActionType::None, InDefaultChord);

#define BIND_COMMAND(CommandName) \
	CommandBindings->MapAction( \
		FGraphPrinterCommands::Get().##CommandName, \
		FExecuteAction::CreateStatic(UGraphPrinterUtils::##CommandName) \
	);

FGraphPrinterCommands::FGraphPrinterCommands()
	: TCommands<FGraphPrinterCommands>
	(
		TEXT("GraphPrinter"),
		NSLOCTEXT("Contexts", "GraphPrinter", "Graph Printer"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
{
}

void FGraphPrinterCommands::RegisterCommands()
{
	REGISTER_COMMAND(PrintGraphWithAllNodes, "Exports all nodes of the currently active graph editor as images.", FInputChord(EKeys::F9, EModifierKey::Control));
	REGISTER_COMMAND(PrintGraphWithSelectedNodes, "Exports the selected node of the currently active graph editor as an image.", FInputChord(EKeys::F10, EModifierKey::Control));
	REGISTER_COMMAND(OpenExportDestinationFolder, "Open the folder containing the images saved by this plugin in Explorer.", FInputChord(EKeys::F12, EModifierKey::Control));
}

void FGraphPrinterCommands::Bind()
{
	if (!FGraphPrinterCommands::IsRegistered())
	{
		UE_LOG(LogGraphPrinter, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
	}

	FGraphPrinterCommands::Get().BindCommands();
}

void FGraphPrinterCommands::BindCommands() const
{
	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();

	BIND_COMMAND(PrintGraphWithAllNodes);
	BIND_COMMAND(PrintGraphWithSelectedNodes);
	BIND_COMMAND(OpenExportDestinationFolder);
}

#undef LOCTEXT_NAMESPACE