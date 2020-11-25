// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCommands.h"
#include "EditorStyleSet.h"

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

#undef LOCTEXT_NAMESPACE