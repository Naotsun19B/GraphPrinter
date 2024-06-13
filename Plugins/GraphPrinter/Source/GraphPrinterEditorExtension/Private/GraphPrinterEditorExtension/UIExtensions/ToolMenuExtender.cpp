// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "GraphPrinterEditorExtension/UIExtensions/AutoToolMenuExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtensionConstants.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#define LOCTEXT_NAMESPACE "ToolMenuExtender"

namespace GraphPrinter
{
	class FToolMenuExtension : public FAutoToolMenuExtender
	{
	private:
		// FAutoToolMenuExtender interface.
		virtual void ExtendToolMenu() override
		{
			if (!UGraphPrinterEditorExtensionSettings::Get().bShowSubMenuInToolMenu)
			{
				return;
			}
			
			auto* ToolMenus = UToolMenus::Get();
			check(IsValid(ToolMenus));
			
			UToolMenu* ToolsMenu = ToolMenus->ExtendMenu(TEXT("MainFrame.MainMenu.Tools"));
			if (!IsValid(ToolsMenu))
			{
				return;
			}

			FToolMenuSection& ToolsSection = ToolsMenu->FindOrAddSection(TEXT("Tools"));
			ToolsSection.AddDynamicEntry(
				Global::PluginName,
				FNewToolMenuSectionDelegate::CreateLambda(
					[](FToolMenuSection& Section)
					{
						Section.AddSubMenu(
							Global::PluginName,
							ToolMenuExtensionConstants::ToolMenuLabel,
							ToolMenuExtensionConstants::ToolMenuTooltip,
							FNewToolMenuDelegate::CreateStatic(&FGraphPrinterCommands::FillMenuBuilder),
							false,
							FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
						);
					}
				)
			);
		}
		// End of FAutoToolMenuExtender interface.
	};
	static FToolMenuExtension ToolMenuExtension;
}
	
#undef LOCTEXT_NAMESPACE
