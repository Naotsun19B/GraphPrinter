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
	
		const TSharedPtr<FUICommandList>& CommandBindings = FGraphPrinterCommands::Get().CommandBindings;
	
		Extender->AddToolBarExtension(
			TEXT("Asset"),
			EExtensionHook::After,
			CommandBindings,
			FToolBarExtensionDelegate::CreateStatic(&FToolbarExtender::OnExtendToolbar)
		);
		FAssetEditorToolkit::GetSharedToolBarExtensibilityManager()->AddExtender(Extender);
	}

	void FToolbarExtender::Unregister()
	{
		FAssetEditorToolkit::GetSharedMenuExtensibilityManager()->RemoveExtender(Extender);
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
		const auto& GraphPrinterCommands = FGraphPrinterCommands::Get();
		
		FMenuBuilder MenuBuilder(true, GraphPrinterCommands.CommandBindings);

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("CopyToClipboardSectionName", "Copy To Clipboard"));
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.CopyGraphWithAllNodesToClipboard);
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.CopyGraphWithSelectedNodesToClipboard);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ExportToImageFileSectionName", "Export To Image File"));
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.PrintGraphWithAllNodes);
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.PrintGraphWithSelectedNodes);
		MenuBuilder.EndSection();

#ifdef WITH_TEXT_CHUNK_HELPER
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("ImportFromImageFileSectionName", "Import From Image File"));
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.RestoreNodesFromPngFile);
		MenuBuilder.EndSection();
#endif
		
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("OtherSectionName", "Other"));
		MenuBuilder.AddMenuEntry(GraphPrinterCommands.OpenExportDestinationFolder);
		MenuBuilder.EndSection();
		
		return MenuBuilder.MakeWidget();
	}

	TSharedPtr<FExtender> FToolbarExtender::Extender = nullptr;
}
	
#undef LOCTEXT_NAMESPACE
