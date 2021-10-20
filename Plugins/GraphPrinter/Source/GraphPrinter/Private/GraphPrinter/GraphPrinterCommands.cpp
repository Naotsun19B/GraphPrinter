// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinter/GraphPrinterCommands.h"
#include "GraphPrinter/GraphPrinterUtils.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrintCommands"

namespace GraphPrinter
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
#if ENABLE_IMAGE_TO_CLIPBOARD
		UI_COMMAND(CopyGraphWithAllNodesToClipboard, "Copy Graph With All Nodes To Clipboard", "Copy all nodes of the currently active graph editor as images to clipboard.", EUserInterfaceActionType::None, FInputChord(EKeys::F7, EModifierKey::Control));
		UI_COMMAND(CopyGraphWithSelectedNodesToClipboard, "Copy Graph With Selected Nodes To Clipboard", "Copy the selected node of the currently active graph editor as an image to clipboard.", EUserInterfaceActionType::None, FInputChord(EKeys::F8, EModifierKey::Control));
#endif
		UI_COMMAND(PrintGraphWithAllNodes, "Print Graph With All Nodes", "Exports all nodes of the currently active graph editor as images.", EUserInterfaceActionType::None, FInputChord(EKeys::F9, EModifierKey::Control));
		UI_COMMAND(PrintGraphWithSelectedNodes, "Print Graph With Selected Nodes", "Exports the selected node of the currently active graph editor as an image.", EUserInterfaceActionType::None, FInputChord(EKeys::F10, EModifierKey::Control));
#if ENABLE_EMBED_NODE_INFO
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
#if ENABLE_IMAGE_TO_CLIPBOARD
		CommandBindings->MapAction(
			CopyGraphWithAllNodesToClipboard,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::CopyGraphWithAllNodesToClipboard)
		);

		CommandBindings->MapAction(
			CopyGraphWithSelectedNodesToClipboard,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::CopyGraphWithSelectedNodesToClipboard)
		);
#endif
	
		CommandBindings->MapAction(
			PrintGraphWithAllNodes,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraphWithAllNodes)
		);

		CommandBindings->MapAction(
			PrintGraphWithSelectedNodes,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::PrintGraphWithSelectedNodes)
		);
	
#if ENABLE_EMBED_NODE_INFO
		CommandBindings->MapAction(
			RestoreNodesFromPngFile,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::RestoreNodesFromPngFile)
		);
#endif

		CommandBindings->MapAction(
			OpenExportDestinationFolder,
			FExecuteAction::CreateStatic(UGraphPrinterUtils::OpenExportDestinationFolder)
		);
	}
}

#undef LOCTEXT_NAMESPACE