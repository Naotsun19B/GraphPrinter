// Copyright 2020-2023 Naotsun. All Rights Reserved.

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
			
			UToolMenu* ToolsMenu = ToolMenus->ExtendMenu(TEXT("AssetEditorToolbar.CommonActions"));
			if (!IsValid(ToolsMenu))
			{
				return;
			}
			
			FToolMenuSection& AssetSection = ToolsMenu->FindOrAddSection(TEXT("Asset"));
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
			)
			.SetCommandList(FGraphPrinterCommands::Get().CommandBindings);
		}
		// End of FAutoToolMenuExtender interface.
	};
	static FToolbarExtension Instance;
}
	
#undef LOCTEXT_NAMESPACE
