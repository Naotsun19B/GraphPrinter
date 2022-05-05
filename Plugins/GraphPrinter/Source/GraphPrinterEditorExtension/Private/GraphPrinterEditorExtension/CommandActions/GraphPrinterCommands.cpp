// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GraphPrinterCommands"

namespace GraphPrinter
{
	FGraphPrinterCommands::FGraphPrinterCommands()
		: TCommands<FGraphPrinterCommands>
		(
			TEXT("GraphPrinter"),
			NSLOCTEXT("Contexts", "GraphPrinter", "Graph Printer"),
			NAME_None,
			FGraphPrinterStyle::Get().GetStyleSetName()
		)
		, CommandBindings(MakeShared<FUICommandList>())
		, bIsBound(false)
	{
	}

	void FGraphPrinterCommands::RegisterCommands()
	{
		// Register command here.
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		UI_COMMAND(CopyGraphWithAllNodesToClipboard, "Copy Graph With All Nodes To Clipboard", "Copy all nodes of the currently active graph editor as images to clipboard.", EUserInterfaceActionType::Button, FInputChord(EKeys::F7, EModifierKey::Control));
		UI_COMMAND(CopyGraphWithSelectedNodesToClipboard, "Copy Graph With Selected Nodes To Clipboard", "Copy the selected node of the currently active graph editor as an image to clipboard.", EUserInterfaceActionType::Button, FInputChord(EKeys::F8, EModifierKey::Control));
#endif
		UI_COMMAND(PrintGraphWithAllNodes, "Print Graph With All Nodes", "Exports all nodes of the currently active graph editor as images.", EUserInterfaceActionType::Button, FInputChord(EKeys::F9, EModifierKey::Control));
		UI_COMMAND(PrintGraphWithSelectedNodes, "Print Graph With Selected Nodes", "Exports the selected node of the currently active graph editor as an image.", EUserInterfaceActionType::Button, FInputChord(EKeys::F10, EModifierKey::Control));
#ifdef WITH_TEXT_CHUNK_HELPER
		UI_COMMAND(RestoreNodesFromPngFile, "Restore Nodes From Png File", "Open the file browser and restore the node from the selected png file.", EUserInterfaceActionType::Button, FInputChord(EKeys::F11, EModifierKey::Control));
#endif
		UI_COMMAND(OpenExportDestinationFolder, "Open Export Destination Folder", "Open the folder containing the images saved by this plugin in Explorer.", EUserInterfaceActionType::Button, FInputChord(EKeys::F12, EModifierKey::Control));
		UI_COMMAND(OpenPluginSettings, "Open Plugin Settings", "Open the Graph Printer settings screen in the editor preferences.", EUserInterfaceActionType::Button, FInputChord());
	}

	bool FGraphPrinterCommands::IsBound()
	{
		check(Instance.IsValid());
		return Instance.Pin()->bIsBound;
	}

	void FGraphPrinterCommands::Bind()
	{
		check(Instance.Pin().IsValid());
		Instance.Pin()->BindCommands();
	}

	void FGraphPrinterCommands::FillMenuBuilder(FMenuBuilder& MenuBuilder)
	{
		const TSharedPtr<FGraphPrinterCommands> This = Instance.Pin();
		check(This.IsValid());
		
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("CopyToClipboardSectionName", "Copy To Clipboard"));
		MenuBuilder.AddMenuEntry(This->CopyGraphWithAllNodesToClipboard);
		MenuBuilder.AddMenuEntry(This->CopyGraphWithSelectedNodesToClipboard);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ExportToImageFileSectionName", "Export To Image File"));
		MenuBuilder.AddMenuEntry(This->PrintGraphWithAllNodes);
		MenuBuilder.AddMenuEntry(This->PrintGraphWithSelectedNodes);
		MenuBuilder.EndSection();

#ifdef WITH_TEXT_CHUNK_HELPER
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ImportFromImageFileSectionName", "Import From Image File"));
		MenuBuilder.AddMenuEntry(This->RestoreNodesFromPngFile);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("OtherSectionName", "Other"));
		MenuBuilder.AddMenuEntry(This->OpenExportDestinationFolder);
		MenuBuilder.AddMenuEntry(This->OpenPluginSettings);
		MenuBuilder.EndSection();
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
			return;
		}
		bIsBound = true;

		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		const TSharedRef<FUICommandList>& MainFrameCommandBindings = MainFrame.GetMainFrameCommandBindings();

		MainFrameCommandBindings->Append(CommandBindings);

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

		CommandBindings->MapAction(
			OpenPluginSettings,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::OpenPluginSettings)
		);
	}
}

#undef LOCTEXT_NAMESPACE
