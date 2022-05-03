// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrintCommands"

namespace GraphPrinterEditorExtension
{
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
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		UI_COMMAND(CopyGraphWithAllNodesToClipboard, "Copy Graph With All Nodes To Clipboard", "Copy all nodes of the currently active graph editor as images to clipboard.", EUserInterfaceActionType::None, FInputChord(EKeys::F7, EModifierKey::Control));
		UI_COMMAND(CopyGraphWithSelectedNodesToClipboard, "Copy Graph With Selected Nodes To Clipboard", "Copy the selected node of the currently active graph editor as an image to clipboard.", EUserInterfaceActionType::None, FInputChord(EKeys::F8, EModifierKey::Control));
#endif
		UI_COMMAND(PrintGraphWithAllNodes, "Print Graph With All Nodes", "Exports all nodes of the currently active graph editor as images.", EUserInterfaceActionType::None, FInputChord(EKeys::F9, EModifierKey::Control));
		UI_COMMAND(PrintGraphWithSelectedNodes, "Print Graph With Selected Nodes", "Exports the selected node of the currently active graph editor as an image.", EUserInterfaceActionType::None, FInputChord(EKeys::F10, EModifierKey::Control));
#ifdef WITH_TEXT_CHUNK_HELPER
		UI_COMMAND(RestoreNodesFromPngFile, "Restore Nodes From Png File", "Open the file browser and restore the node from the selected png file.", EUserInterfaceActionType::None, FInputChord(EKeys::F11, EModifierKey::Control));
#endif
		UI_COMMAND(OpenExportDestinationFolder, "Open Export Destination Folder", "Open the folder containing the images saved by this plugin in Explorer.", EUserInterfaceActionType::None, FInputChord(EKeys::F12, EModifierKey::Control));
	}

	bool FGraphPrinterCommands::IsBound()
	{
		check(Instance.IsValid());
		return Instance.Pin()->bIsBound;
	}

	void FGraphPrinterCommands::Bind()
	{
		Instance.Pin()->BindCommands();
	}

	void FGraphPrinterCommands::BindCommands()
	{
		if (!IsRegistered())
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("Bound before UI Command was registered.\nPlease be sure to bind after registration."));
		}

		if (IsBound())
		{
			UE_LOG(LogGraphPrinter, Warning, TEXT("The binding process has already been completed."));
		}
		bIsBound = true;

		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		const TSharedRef<FUICommandList>& CommandBindings = MainFrame.GetMainFrameCommandBindings();

		// Bind command here.
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		CommandBindings->MapAction(
			CopyGraphWithAllNodesToClipboard,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CopyGraphWithAllNodesToClipboard)
		);

		CommandBindings->MapAction(
			CopyGraphWithSelectedNodesToClipboard,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CopyGraphWithSelectedNodesToClipboard)
		);
#endif
	
		CommandBindings->MapAction(
			PrintGraphWithAllNodes,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::PrintGraphWithAllNodes)
		);

		CommandBindings->MapAction(
			PrintGraphWithSelectedNodes,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::PrintGraphWithSelectedNodes)
		);
	
#ifdef WITH_TEXT_CHUNK_HELPER
		CommandBindings->MapAction(
			RestoreNodesFromPngFile,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::RestoreNodesFromPngFile)
		);
#endif

		CommandBindings->MapAction(
			OpenExportDestinationFolder,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::OpenExportDestinationFolder)
		);
	}
}

#undef LOCTEXT_NAMESPACE
