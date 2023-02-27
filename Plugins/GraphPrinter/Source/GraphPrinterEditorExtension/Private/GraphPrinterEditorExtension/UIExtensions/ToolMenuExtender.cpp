// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/StreamDeckMenu.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

#define LOCTEXT_NAMESPACE "ToolMenuExtender"

namespace GraphPrinter
{
	namespace LevelEditor
	{
		static TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager()
		{
			auto& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
			return LevelEditor.GetMenuExtensibilityManager();
		}
	}
	
	void FToolMenuExtender::Register()
	{
		if (Extender.IsValid())
		{
			return;
		}
		
		Extender = MakeShared<FExtender>();
		check(Extender.IsValid());
	
		const TSharedPtr<FUICommandList>& CommandBindings = FGraphPrinterCommands::Get().CommandBindings;

		const FName ExtensionHook =
#if UE_5_00_OR_LATER
			TEXT("Tools");
#else
			TEXT("GraphEditor");
#endif
		
		Extender->AddMenuExtension(
			ExtensionHook,
			EExtensionHook::After,
			CommandBindings,
			FMenuExtensionDelegate::CreateStatic(&FToolMenuExtender::OnExtendToolMenu)
		);

		const TArray<TSharedPtr<FExtensibilityManager>, TInlineAllocator<2>> ExtensibilityManagers = {
			FAssetEditorToolkit::GetSharedMenuExtensibilityManager(),
			LevelEditor::GetMenuExtensibilityManager()
		};
		for (const auto& ExtensibilityManager : ExtensibilityManagers)
		{
			if (ExtensibilityManager.IsValid())
			{
				ExtensibilityManager->AddExtender(Extender);
			}
		}
	}

	void FToolMenuExtender::Unregister()
	{
		const TArray<TSharedPtr<FExtensibilityManager>, TInlineAllocator<2>> ExtensibilityManagers = {
			FAssetEditorToolkit::GetSharedMenuExtensibilityManager(),
			LevelEditor::GetMenuExtensibilityManager()
		};
		for (const auto& ExtensibilityManager : ExtensibilityManagers)
		{
			if (ExtensibilityManager.IsValid())
			{
				ExtensibilityManager->RemoveExtender(Extender);
			}
		}
	}

	void FToolMenuExtender::OnExtendToolMenu(FMenuBuilder& MenuBuilder)
	{
#if !UE_5_00_OR_LATER
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ToolsSectionTitle", "Tools"));
#endif
		MenuBuilder.AddSubMenu(
			LOCTEXT("SubMenuTitle", "Graph Printer"),
			LOCTEXT("SubMenuDescription", "You can perform the functions of Gprah Printer from here."),
			FNewMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendSubMenu),
			false,
			FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
		);
#if !UE_5_00_OR_LATER
		MenuBuilder.EndSection();
#endif
	}

	void FToolMenuExtender::OnExtendSubMenu(FMenuBuilder& MenuBuilder)
	{
		FGraphPrinterCommands::FillMenuBuilder(MenuBuilder);
#ifdef WITH_STREAM_DECK
		FStreamDeckMenu::FillMenuBuilder(MenuBuilder);
#endif
	}

	TSharedPtr<FExtender> FToolMenuExtender::Extender = nullptr;
}
	
#undef LOCTEXT_NAMESPACE
