// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "GraphPrinterEditorExtension/UIExtensions/AutoToolMenuExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtensionConstants.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IMainFrameModule.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Toolkits/AssetEditorToolkit.h"

#define LOCTEXT_NAMESPACE "StatusBarExtender"

#if UE_5_00_OR_LATER
namespace GraphPrinter
{
	class FStatusBarExtension : public FAutoToolMenuExtender
	{
	private:
		// FAutoToolMenuExtender interface.
		virtual void ExtendToolMenu() override
		{
			if (!UGraphPrinterEditorExtensionSettings::Get().bShowComboButtonInStatusBar)
			{
				return;
			}

			ExtendStatusBar(TEXT("LevelEditor.StatusBar.ToolBar"));
			
			IMainFrameModule::Get().OnMainFrameCreationFinished().AddStatic(&FStatusBarExtension::HandleOnMainFrameCreationFinished);
		}
		// End of FAutoToolMenuExtender interface.

		// Called when the editor mainframe has been created.
		static void HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
		{
			if (auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				AssetEditorSubsystem->OnEditorOpeningPreWidgets().AddStatic(&FStatusBarExtension::HandleOnEditorOpeningPreWidgets);
			}
		}

		// Called when an asset editor is opening and before widgets are constructed.
		static void HandleOnEditorOpeningPreWidgets(const TArray<UObject*>& AssetsToEdit, IAssetEditorInstance* AssetEditorInstance)
		{
			if (AssetEditorInstance == nullptr)
			{
				return;
			}
			
			FName BuiltStatusBarName;
			{
				const auto* AssetEditorToolkit = static_cast<FAssetEditorToolkit*>(AssetEditorInstance);
				check(AssetEditorToolkit != nullptr);
				const TSharedRef<IToolkitHost>& ToolkitHost = AssetEditorToolkit->GetToolkitHost();
				
				const FString StatusBarName = ToolkitHost->GetStatusBarName().ToString();
				FString AssetEditorName;
				if (!StatusBarName.Split(TEXT("_"), &AssetEditorName, nullptr))
				{
					return;
				}

				BuiltStatusBarName = FName(AssetEditorName + TEXT(".ToolBar"));
			}
			ExtendStatusBar(BuiltStatusBarName);
		}

		// Called when the menu is expanded.
		static void ExtendStatusBar(const FName& StatusBarName)
		{
			auto* ToolMenus = UToolMenus::Get();
			if (!IsValid(ToolMenus))
			{
				return;
			}
			
			UToolMenu* ToolMenu = ToolMenus->ExtendMenu(StatusBarName);
			if (!IsValid(ToolMenu))
			{
				return;
			}
			
			FToolMenuSection& ToolMenuSection = ToolMenu->AddSection(Global::PluginName);
			ToolMenuSection.AddEntry(
				FToolMenuEntry::InitComboButton(
					Global::PluginName,
					FToolUIActionChoice(),
					FNewToolMenuChoice(
						FNewToolMenuDelegate::CreateStatic(&FGraphPrinterCommands::FillMenuBuilder)
					),
					FText::GetEmpty(), // Currently, if I don't create a combo box myself using Slate, the label will be buried, so I can only use the icon.
					ToolMenuExtensionConstants::ToolMenuTooltip,
					FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType::PluginIcon)
				)
			);
		}
	};
	static FStatusBarExtension StatusBarExtension;
}
#endif
	
#undef LOCTEXT_NAMESPACE
