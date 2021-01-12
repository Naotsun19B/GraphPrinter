// Copyright 2020 Naotsun. All Rights Reserved.

#include "GraphPrinterCommands.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterUtils.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrintCommands"

FGraphPrinterCommands::FGraphPrinterCommands()
	: TCommands<FGraphPrinterCommands>
	(
		TEXT("GraphPrinter"),
		NSLOCTEXT("Contexts", "GraphPrinter", "Graph Printer"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
	, bIsBound(false)
{
}

void FGraphPrinterCommands::RegisterCommands()
{
	// Register command here.
	UI_COMMAND(PrintGraphWithAllNodes, "Print Graph With All Nodes", "Exports all nodes of the currently active graph editor as images.", EUserInterfaceActionType::None, FInputChord(EKeys::F9, EModifierKey::Control));
	UI_COMMAND(PrintGraphWithSelectedNodes, "Print Graph With Selected Nodes", "Exports the selected node of the currently active graph editor as an image.", EUserInterfaceActionType::None, FInputChord(EKeys::F10, EModifierKey::Control));
#ifdef ENABLE_EMBED_NODE_INFO
	UI_COMMAND(RestoreNodesFromPngFile, "Restore Nodes From Png File", "Open the file browser and restore the node from the selected png file.", EUserInterfaceActionType::None, FInputChord(EKeys::F11, EModifierKey::Control));
#endif
	UI_COMMAND(OpenExportDestinationFolder, "Open Export Destination Folder", "Open the folder containing the images saved by this plugin in Explorer.", EUserInterfaceActionType::None, FInputChord(EKeys::F12, EModifierKey::Control));
}

bool FGraphPrinterCommands::IsBound()
{
	return Instance.Pin()->bIsBound;
}

void FGraphPrinterCommands::Bind()
{
	Instance.Pin()->BindCommands();
}

void FGraphPrinterCommands::BindCommands()
{
	if (!FGraphPrinterCommands::IsRegistered())
	{
		UE_LOG(LogGraphPrinter, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
	}

	if (FGraphPrinterCommands::IsBound())
	{
		UE_LOG(LogGraphPrinter, Warning, TEXT("The binding process has already been completed."));
	}
	bIsBound = true;

	IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();

	// Bind command here.
	CommandBindings->MapAction(
		FGraphPrinterCommands::Get().PrintGraphWithAllNodes,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraphWithAllNodes)
	);

	CommandBindings->MapAction(
		FGraphPrinterCommands::Get().PrintGraphWithSelectedNodes,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraphWithSelectedNodes)
	);

#ifdef ENABLE_EMBED_NODE_INFO
	CommandBindings->MapAction(
		FGraphPrinterCommands::Get().RestoreNodesFromPngFile,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::RestoreNodesFromPngFile)
	);
#endif

	CommandBindings->MapAction(
		FGraphPrinterCommands::Get().OpenExportDestinationFolder,
		FExecuteAction::CreateStatic(UGraphPrinterUtils::OpenExportDestinationFolder)
	);
}

#undef LOCTEXT_NAMESPACE