// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtender.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

#define LOCTEXT_NAMESPACE "ToolMenuExtender"

namespace GraphPrinter
{
	void FToolMenuExtender::Register()
	{
		Extender = MakeShared<FExtender>();
		check(Extender.IsValid());
	
		const TSharedPtr<FUICommandList>& CommandBindings = FGraphPrinterCommands::Get().CommandBindings;

		const FName ExtensionHook =
#if BEFORE_UE_4_27
			TEXT("GraphEditor");
#else
			TEXT("Tools");
#endif
		
		Extender->AddMenuExtension(
			ExtensionHook,
			EExtensionHook::After,
			CommandBindings,
			FMenuExtensionDelegate::CreateStatic(&FToolMenuExtender::OnExtendToolMenu)
		);
		
		const TSharedPtr<FExtensibilityManager>& ExtensibilityManager = FAssetEditorToolkit::GetSharedMenuExtensibilityManager();
		if (ExtensibilityManager.IsValid())
		{
			ExtensibilityManager->AddExtender(Extender);
		}
	}

	void FToolMenuExtender::Unregister()
	{
		const TSharedPtr<FExtensibilityManager>& ExtensibilityManager = FAssetEditorToolkit::GetSharedMenuExtensibilityManager();
		if (ExtensibilityManager.IsValid())
		{
			ExtensibilityManager->RemoveExtender(Extender);
		}
	}

	void FToolMenuExtender::OnExtendToolMenu(FMenuBuilder& MenuBuilder)
	{
#if BEFORE_UE_4_27
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ToolsSectionTitle", "Tools"));
#endif
		MenuBuilder.AddSubMenu(
			LOCTEXT("SubMenuTitle", "Graph Printer"),
			LOCTEXT("SubMenuDescription", "You can perform the functions of Gprah Printer from here."),
			FNewMenuDelegate::CreateStatic(&FToolMenuExtender::OnExtendSubMenu),
			false,
			FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
		);
#if BEFORE_UE_4_27
		MenuBuilder.EndSection();
#endif
	}

	void FToolMenuExtender::OnExtendSubMenu(FMenuBuilder& MenuBuilder)
	{
		FGraphPrinterCommands::FillMenuBuilder(MenuBuilder);
	}

	TSharedPtr<FExtender> FToolMenuExtender::Extender = nullptr;
}
	
#undef LOCTEXT_NAMESPACE
