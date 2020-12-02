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
	, bIsBound(false)
{
}

void FGraphPrinterCommands::RegisterCommands()
{
	FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();
	if (SlateRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Fatal, TEXT("The command cannot be bound because SlateRender cannot be obtained."));
	}
	SlateWindowRenderedHandle = SlateRenderer->OnSlateWindowRendered().AddRaw(this, &FGraphPrinterCommands::HandleOnSlateWindowRendered);

	// Register command here.
	REGISTER_COMMAND(PrintGraphWithAllNodes, "Exports all nodes of the currently active graph editor as images.", FInputChord(EKeys::F9, EModifierKey::Control));
	REGISTER_COMMAND(PrintGraphWithSelectedNodes, "Exports the selected node of the currently active graph editor as an image.", FInputChord(EKeys::F10, EModifierKey::Control));
	REGISTER_COMMAND(OpenExportDestinationFolder, "Open the folder containing the images saved by this plugin in Explorer.", FInputChord(EKeys::F12, EModifierKey::Control));
}

bool FGraphPrinterCommands::IsBound()
{
	return FGraphPrinterCommands::Get().bIsBound;
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
	BIND_COMMAND(PrintGraphWithAllNodes);
	BIND_COMMAND(PrintGraphWithSelectedNodes);
	BIND_COMMAND(OpenExportDestinationFolder);
}

void FGraphPrinterCommands::HandleOnSlateWindowRendered(SWindow& SlateWindow, void* ViewportRHIPtr)
{
	if (GEditor == nullptr || !IsInGameThread())
	{
		const FString& FunctionName = GET_FUNCTION_NAME_STRING_CHECKED(FGraphPrinterCommands, HandleOnSlateWindowRendered);
		UE_LOG(LogGraphPrinter, Fatal, TEXT("%s was called by someone other than GameThread."), *FunctionName);
	}

	FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();
	if (SlateRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Fatal, TEXT("The command cannot be bound because SlateRender cannot be obtained."));
	}
	SlateRenderer->OnSlateWindowRendered().Remove(SlateWindowRenderedHandle);

	BindCommands();
}

#undef LOCTEXT_NAMESPACE