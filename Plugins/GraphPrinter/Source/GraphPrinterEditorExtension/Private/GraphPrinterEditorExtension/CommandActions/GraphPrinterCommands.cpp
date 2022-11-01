// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "Interfaces/IMainFrameModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

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
		UI_COMMAND(CopyAllAreaOfWidgetToClipboard, "Copy All Area Of Widget To Clipboard", "Copy the entire target widget as an image to the clipboard.", EUserInterfaceActionType::Button, FInputChord(EKeys::F7, EModifierKey::Control));
		UI_COMMAND(CopySelectedAreaOfWidgetToClipboard, "Copy Selected Area Of Widget To Clipboard", "Copy the selected area of the target widget to the clipboard as an image.", EUserInterfaceActionType::Button, FInputChord(EKeys::F8, EModifierKey::Control));
#endif
		UI_COMMAND(PrintAllAreaOfWidget, "Print All Area Of Widget", "Outputs the entire target widget as an image file.", EUserInterfaceActionType::Button, FInputChord(EKeys::F9, EModifierKey::Control));
		UI_COMMAND(PrintSelectedAreaOfWidget, "Print Selected Area Of Widget", "Outputs the selected area of the target widget as an image file.", EUserInterfaceActionType::Button, FInputChord(EKeys::F10, EModifierKey::Control));
#ifdef WITH_TEXT_CHUNK_HELPER
		UI_COMMAND(RestoreWidgetFromImageFile, "Restore Widget From Image File", "Restores the state of the widget from the image file.\nYou can only restore from images output from this plugin.", EUserInterfaceActionType::Button, FInputChord(EKeys::F11, EModifierKey::Control));
#endif
		UI_COMMAND(OpenExportDestinationFolder, "Open Export Destination Folder", "Open the folder containing the images saved by this plugin in Explorer.", EUserInterfaceActionType::Button, FInputChord(EKeys::F12, EModifierKey::Control));
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
		MenuBuilder.AddMenuEntry(This->CopyAllAreaOfWidgetToClipboard);
		MenuBuilder.AddMenuEntry(This->CopySelectedAreaOfWidgetToClipboard);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ExportToImageFileSectionName", "Export To Image File"));
		MenuBuilder.AddMenuEntry(This->PrintAllAreaOfWidget);
		MenuBuilder.AddMenuEntry(This->PrintSelectedAreaOfWidget);
		MenuBuilder.EndSection();

#ifdef WITH_TEXT_CHUNK_HELPER
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ImportFromImageFileSectionName", "Import From Image File"));
		MenuBuilder.AddMenuEntry(This->RestoreWidgetFromImageFile);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("OtherSectionName", "Other"));
		MenuBuilder.AddMenuEntry(This->OpenExportDestinationFolder);
		MenuBuilder.AddSubMenu(
			LOCTEXT("OpenPluginSettingsSubMenuTitle", "Open Plugin Settings"),
			LOCTEXT("OpenPluginSettingsSubMenuTooltip", "Open the Graph Printer settings screen in the editor preferences."),
			FNewMenuDelegate::CreateStatic(&FGraphPrinterCommands::OnExtendOpenSettingsSubMenu)
		);
		MenuBuilder.EndSection();
	}

	TSharedPtr<FUICommandInfo> FGraphPrinterCommands::FindCommandByName(const FName& CommandName) const
	{
		static constexpr int32 NumOfCommands = (
			3
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
			+ 2
#endif
#ifdef WITH_TEXT_CHUNK_HELPER
			+ 1
#endif
		);
		
		static const TArray<TSharedPtr<FUICommandInfo>, TInlineAllocator<NumOfCommands>> Commands = {
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
			CopyAllAreaOfWidgetToClipboard,
			CopySelectedAreaOfWidgetToClipboard,
#endif
			PrintAllAreaOfWidget,
			PrintSelectedAreaOfWidget,
#ifdef WITH_TEXT_CHUNK_HELPER
			RestoreWidgetFromImageFile,
#endif
			OpenExportDestinationFolder,
		};

		for (const auto& Command : Commands)
		{
			if (!Command.IsValid())
			{
				continue;
			}

			if (Command->GetCommandName().IsEqual(CommandName))
			{
				return Command;
			}
		}

		return nullptr;
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
		
		const TSharedRef<FUICommandList>& MainFrameCommandBindings = IMainFrameModule::Get().GetMainFrameCommandBindings();

		MainFrameCommandBindings->Append(CommandBindings);

		// Bind command here.
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		CommandBindings->MapAction(
			CopyAllAreaOfWidgetToClipboard,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CopyAllAreaOfWidgetToClipboard),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanCopyAllAreaOfWidgetToClipboard)
		);

		CommandBindings->MapAction(
			CopySelectedAreaOfWidgetToClipboard,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CopySelectedAreaOfWidgetToClipboard),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanCopySelectedAreaOfWidgetToClipboard)
		);
#endif
	
		CommandBindings->MapAction(
			PrintAllAreaOfWidget,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::PrintAllAreaOfWidget),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanPrintAllAreaOfWidget)
		);

		CommandBindings->MapAction(
			PrintSelectedAreaOfWidget,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::PrintSelectedAreaOfWidget),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanPrintSelectedAreaOfWidget)
		);
	
#ifdef WITH_TEXT_CHUNK_HELPER
		CommandBindings->MapAction(
			RestoreWidgetFromImageFile,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::RestoreWidgetFromImageFile),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanExecuteRestoreWidget)
		);
#endif

		CommandBindings->MapAction(
			OpenExportDestinationFolder,
			FExecuteAction::CreateStatic(&FGraphPrinterCommandActions::OpenExportDestinationFolder),
			FCanExecuteAction::CreateStatic(&FGraphPrinterCommandActions::CanOpenExportDestinationFolder)
		);
	}

	void FGraphPrinterCommands::OnExtendOpenSettingsSubMenu(FMenuBuilder& MenuBuilder)
	{
		const TArray<UGraphPrinterSettings::FSettingsInfo>& AllSettings = UGraphPrinterSettings::GetAllSettings();
		for (const auto& Setting : AllSettings)
		{
			MenuBuilder.AddMenuEntry(
				Setting.DisplayName,
				Setting.Description,
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateStatic(&UGraphPrinterSettings::OpenSettings, Setting.SectionName)
				)
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
