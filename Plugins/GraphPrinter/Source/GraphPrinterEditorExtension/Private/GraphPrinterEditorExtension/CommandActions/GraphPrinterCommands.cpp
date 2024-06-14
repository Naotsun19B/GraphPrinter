// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "WidgetPrinter/ISupportedWidgetRegistry.h"
#ifdef WITH_STREAM_DECK
#include "GraphPrinterStreamDeck/HAL/StreamDeckUtils.h"
#endif
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "Interfaces/IMainFrameModule.h"
#include "ToolMenu.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPrinterCommands"

namespace GraphPrinter
{
	FGraphPrinterCommands::FGraphPrinterCommands()
		: TCommands<FGraphPrinterCommands>
		(
			TEXT("GraphPrinter"),
			LOCTEXT("ContextDesc", "Graph Printer"),
			NAME_None,
			FGraphPrinterStyle::Get().GetStyleSetName()
		)
		, CommandBindings(MakeShared<FUICommandList>())
		, bIsBound(false)
	{
	}

	void FGraphPrinterCommands::RegisterCommands()
	{
		UI_COMMAND(CollectTargetWidgets, "Collect Target Widgets", "Collects widgets supported by any printer from the currently displayed screen.", EUserInterfaceActionType::Button, FInputChord(EKeys::F6, EModifierKey::Control));
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		UI_COMMAND(CopyAllAreaOfWidgetToClipboard, "Copies All Area Of Widget To Clipboard", "Copy the entire target widget as an image to the clipboard.", EUserInterfaceActionType::Button, FInputChord(EKeys::F7, EModifierKey::Control));
		UI_COMMAND(CopySelectedAreaOfWidgetToClipboard, "Copies Selected Area Of Widget To Clipboard", "Copy the selected area of the target widget to the clipboard as an image.", EUserInterfaceActionType::Button, FInputChord(EKeys::F8, EModifierKey::Control));
#endif
		UI_COMMAND(PrintAllAreaOfWidget, "Prints All Area Of Widget", "Outputs the entire target widget as an image file.", EUserInterfaceActionType::Button, FInputChord(EKeys::F9, EModifierKey::Control));
		UI_COMMAND(PrintSelectedAreaOfWidget, "Prints Selected Area Of Widget", "Outputs the selected area of the target widget as an image file.", EUserInterfaceActionType::Button, FInputChord(EKeys::F10, EModifierKey::Control));
#ifdef WITH_TEXT_CHUNK_HELPER
		UI_COMMAND(RestoreWidgetFromImageFile, "Restores Widget From Image File", "Restores the state of the widget from the image file.\nYou can only restore from images output from this plugin.", EUserInterfaceActionType::Button, FInputChord(EKeys::F11, EModifierKey::Control));
#endif
		UI_COMMAND(OpenExportDestinationFolder, "Opens Export Destination Folder", "Open the folder containing the images saved by this plugin in Explorer.", EUserInterfaceActionType::Button, FInputChord(EKeys::F12, EModifierKey::Control));
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

	void FGraphPrinterCommands::FillMenuBuilder(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}
		
		const TSharedPtr<FGraphPrinterCommands> This = Instance.Pin();
		check(This.IsValid());

		ToolMenu->Context.AppendCommandList(This->CommandBindings);
		
		{
			FToolMenuSection& TargetSection = ToolMenu->AddSection(
				TEXT("Target"),
				LOCTEXT("TargetSectionName", "Target")
			);

			auto& SupportedWidgetRegistry = ISupportedWidgetRegistry::Get();
			
			if (UGraphPrinterEditorExtensionSettings::Get().bCollectTargetWidgetsAutomatically)
			{
				SupportedWidgetRegistry.CollectSupportedWidget();
			}

			TargetSection.AddMenuEntry(This->CollectTargetWidgets);
			
			const TArray<FSupportedWidget>& SupportedWidgets = SupportedWidgetRegistry.GetSupportedWidgets();
			for (const auto& SupportedWidget : SupportedWidgets)
			{
				TargetSection.AddMenuEntry(
					*SupportedWidget.GetIdentifier().ToString(),
					SupportedWidget.GetDisplayName(),
					FText::GetEmpty(),
					FSlateIcon(),
					FToolUIActionChoice(
						FUIAction(
							FExecuteAction::CreateRaw(&ISupportedWidgetRegistry::Get(), &ISupportedWidgetRegistry::SetSelectedWidget, SupportedWidget.GetIdentifier()),
							FCanExecuteAction(),
							FIsActionChecked::CreateLambda(
								[&SupportedWidget]() -> bool
								{
									const TOptional<FSupportedWidget>& SelectedWidget = ISupportedWidgetRegistry::Get().GetSelectedWidget();
									return (SelectedWidget.IsSet() && (SelectedWidget.GetValue() == SupportedWidget));
								}
							)
						)
					),
					EUserInterfaceActionType::ToggleButton
				);
			}
		}
		
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		{
			FToolMenuSection& CopyToClipboardSection = ToolMenu->AddSection(
				TEXT("CopyToClipboard"),
				LOCTEXT("CopyToClipboardSectionName", "Copy To Clipboard")
			);
			CopyToClipboardSection.AddMenuEntry(This->CopyAllAreaOfWidgetToClipboard);
			CopyToClipboardSection.AddMenuEntry(This->CopySelectedAreaOfWidgetToClipboard);
		}
#endif
		{
			FToolMenuSection& ExportToImageFileSection = ToolMenu->AddSection(
				TEXT("ExportToImageFile"),
				LOCTEXT("ExportToImageFileSectionName", "Export To Image File")
			);
			ExportToImageFileSection.AddMenuEntry(This->PrintAllAreaOfWidget);
			ExportToImageFileSection.AddMenuEntry(This->PrintSelectedAreaOfWidget);
		}
#ifdef WITH_TEXT_CHUNK_HELPER
		{
			FToolMenuSection& ImportFromImageFileSection = ToolMenu->AddSection(
				TEXT("ImportFromImageFile"),
				LOCTEXT("ImportFromImageFileSectionName", "Import From Image File")
			);
			ImportFromImageFileSection.AddMenuEntry(This->RestoreWidgetFromImageFile);
		}
#endif
		{
			FToolMenuSection& OtherSection = ToolMenu->AddSection(
				TEXT("Other"),
				LOCTEXT("OtherSectionName", "Other")
			);
			OtherSection.AddMenuEntry(This->OpenExportDestinationFolder);
			OtherSection.AddSubMenu(
				TEXT("OpenPluginSettings"),
				LOCTEXT("OpenPluginSettingsSubMenuTitle", "Open Plugin Settings"),
				LOCTEXT("OpenPluginSettingsSubMenuTooltip", "Open the Graph Printer settings screen in the editor preferences."),
				FNewToolMenuDelegate::CreateStatic(&FGraphPrinterCommands::OnExtendOpenSettingsSubMenu),
				false,
				FSlateIcon(
#if UE_5_00_OR_LATER
					FAppStyle::GetAppStyleSetName(),
#else
					FEditorStyle::GetStyleSetName(),
#endif
					TEXT("Icons.Settings")
				)
			);
#ifdef WITH_STREAM_DECK
			if (FStreamDeckUtils::IsStreamDeckInstalled())
			{
				OtherSection.AddMenuEntry(
					TEXT("StreamDeck"),
					LOCTEXT("InstallPluginTitle", "Install Stream Deck Plugin"),
					LOCTEXT("InstallPluginTooltip", "Install the Stream Deck plugin that works with this plugin on the Stream Deck.\nIf this item fails to run, it is already installed.\nIf you don't see the Graph Printer item in the Stream Deck application, restart the Stream Deck application."),
					FSlateIcon(
#if UE_5_00_OR_LATER
						FAppStyle::GetAppStyleSetName(),
#else
						FEditorStyle::GetStyleSetName(),
#endif
						TEXT("Icons.Download")
					),
					FUIAction(
						FExecuteAction::CreateStatic(&FStreamDeckUtils::InstallStreamDeckPlugin),
						FCanExecuteAction::CreateLambda(
							[]() -> bool
							{
								return !FStreamDeckUtils::IsInstalledStreamDeckPlugin();
							}
						)
					)
				);
			}
#endif
		}
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

		CommandBindings->MapAction(
			CollectTargetWidgets,
			FExecuteAction::CreateRaw(&ISupportedWidgetRegistry::Get(), &ISupportedWidgetRegistry::CollectSupportedWidget)
		);

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

	void FGraphPrinterCommands::OnExtendOpenSettingsSubMenu(UToolMenu* ToolMenu)
	{
		if (!IsValid(ToolMenu))
		{
			return;
		}

		FToolMenuSection& SettingsSection = ToolMenu->AddSection(TEXT("Settings"));
		
		const TArray<UGraphPrinterSettings::FSettingsInfo>& AllSettings = UGraphPrinterSettings::GetAllSettings();
		for (const auto& Setting : AllSettings)
		{
			SettingsSection.AddMenuEntry(
				Setting.SectionName,
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
