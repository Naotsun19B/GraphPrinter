// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolbarExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolMenuExtender.h"

namespace GraphPrinter
{
	class FGraphPrinterEditorExtensionModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.
	};

	void FGraphPrinterEditorExtensionModule::StartupModule()
	{
		// Register style set.
		FGraphPrinterStyle::Register();
		
		// Register command actions.
		FGraphPrinterCommands::Register();
		FGraphPrinterCommands::Bind();
		
		// Register UI extension.
		FToolbarExtender::Register();
		FToolMenuExtender::Register();
	}

	void FGraphPrinterEditorExtensionModule::ShutdownModule()
	{
		// Unregister UI extension.
		FToolMenuExtender::Unregister();
		FToolbarExtender::Unregister();
		
		// Unregister command actions.
		FGraphPrinterCommands::Unregister();

		// Unregister style set.
		FGraphPrinterStyle::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterEditorExtensionModule, GraphPrinterEditorExtension)
