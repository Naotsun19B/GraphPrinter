// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/ToolbarExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "Toolkits/AssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "ToolbarExtender"

namespace GraphPrinterEditorExtension
{
	void FToolbarExtender::Register()
	{
		Extender = MakeShared<FExtender>();
		check(Extender.IsValid());
		
		Extender->AddToolBarExtension(
			TEXT("Asset"),
			EExtensionHook::After,
			nullptr,
			FToolBarExtensionDelegate::CreateStatic(&FToolbarExtender::OnExtendToolbar)
		);

		const TSharedPtr<FExtensibilityManager> ExtensibilityManager = FAssetEditorToolkit::GetSharedMenuExtensibilityManager();
		if (ExtensibilityManager.IsValid())
		{
			ExtensibilityManager->AddExtender(Extender);
		}
	}

	void FToolbarExtender::Unregister()
	{
		const TSharedPtr<FExtensibilityManager> ExtensibilityManager = FAssetEditorToolkit::GetSharedMenuExtensibilityManager();
		if (ExtensibilityManager.IsValid())
		{
			ExtensibilityManager->RemoveExtender(Extender);
		}
	}

	void FToolbarExtender::OnExtendToolbar(FToolBarBuilder& ToolBarBuilder)
	{
		ToolBarBuilder.AddSeparator();
		ToolBarBuilder.AddComboButton(
			FUIAction(),
			FOnGetContent::CreateStatic(&FToolbarExtender::OnGetComboBoxContent),
			LOCTEXT("ComboButtonTitle", "Graph Printer"),
			LOCTEXT("ComboButtonDescription", "You can perform the functions of Gprah Printer from here."),
			FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
		);
	}

	TSharedRef<SWidget> FToolbarExtender::OnGetComboBoxContent()
	{
		const auto& Commands = FGraphPrinterCommands::Get();
		
		FMenuBuilder MenuBuilder(true, nullptr);

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		MenuBuilder.AddSeparator();
		MenuBuilder.AddMenuEntry(Commands.CopyGraphWithAllNodesToClipboard);
		MenuBuilder.AddMenuEntry(Commands.CopyGraphWithSelectedNodesToClipboard);
#endif

		MenuBuilder.AddSeparator();
		MenuBuilder.AddMenuEntry(Commands.PrintGraphWithAllNodes);
		MenuBuilder.AddMenuEntry(Commands.PrintGraphWithSelectedNodes);

#ifdef WITH_TEXT_CHUNK_HELPER
		MenuBuilder.AddSeparator();
		MenuBuilder.AddMenuEntry(Commands.RestoreNodesFromPngFile);
#endif

		MenuBuilder.AddSeparator();
		MenuBuilder.AddMenuEntry(Commands.OpenExportDestinationFolder);
		
		return MenuBuilder.MakeWidget();
	}

	TSharedPtr<FExtender> FToolbarExtender::Extender = nullptr;
}

#undef LOCTEXT_NAMESPACE
