// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/ToolbarExtender.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
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
		
		const TSharedPtr<FExtensibilityManager>& ExtensibilityManager = FAssetEditorToolkit::GetSharedToolBarExtensibilityManager();
		if (ExtensibilityManager.IsValid())
		{
			ExtensibilityManager->AddExtender(Extender);
		}
	}

	void FToolbarExtender::Unregister()
	{
		const TSharedPtr<FExtensibilityManager>& ExtensibilityManager = FAssetEditorToolkit::GetSharedToolBarExtensibilityManager();
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
		FMenuBuilder MenuBuilder(true, FGraphPrinterCommands::Get().CommandBindings);
		FGraphPrinterCommands::FillMenuBuilder(MenuBuilder);
		return MenuBuilder.MakeWidget();
	}

	TSharedPtr<FExtender> FToolbarExtender::Extender = nullptr;
}
	
#undef LOCTEXT_NAMESPACE
