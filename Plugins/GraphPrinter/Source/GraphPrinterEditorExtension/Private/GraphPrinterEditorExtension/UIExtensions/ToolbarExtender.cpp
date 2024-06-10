// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "GraphPrinterEditorExtension/UIExtensions/AutoToolMenuExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtensionConstants.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#define LOCTEXT_NAMESPACE "ToolbarExtender"

namespace GraphPrinter
{
	class FToolbarExtension : public FAutoToolMenuExtender
	{
	private:
		// FAutoToolMenuExtender interface.
		virtual void ExtendToolMenu() override
		{
			if (!UGraphPrinterEditorExtensionSettings::Get().bShowComboButtonInToolbar)
			{
				return;
			}
			
			auto* ToolMenus = UToolMenus::Get();
			check(IsValid(ToolMenus));
			
			UToolMenu* AssetEditorToolbarMenu = ToolMenus->ExtendMenu(TEXT("AssetEditor.DefaultToolBar"));
			if (!IsValid(AssetEditorToolbarMenu))
			{
				return;
			}

			FToolMenuSection& AssetSection = AssetEditorToolbarMenu->FindOrAddSection(TEXT("Asset"));
			AssetSection.AddEntry(
				FToolMenuEntry::InitComboButton(
					Global::PluginName,
					FToolUIActionChoice(),
					FNewToolMenuChoice(
						FNewToolMenuDelegate::CreateStatic(&FGraphPrinterCommands::FillMenuBuilder)
					),
					ToolMenuExtensionConstants::ToolMenuLabel,
					ToolMenuExtensionConstants::ToolMenuTooltip,
					FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
				)
			);
		}
		// End of FAutoToolMenuExtender interface.
	};
	static FToolbarExtension ToolbarExtension;
}
	
#undef LOCTEXT_NAMESPACE
