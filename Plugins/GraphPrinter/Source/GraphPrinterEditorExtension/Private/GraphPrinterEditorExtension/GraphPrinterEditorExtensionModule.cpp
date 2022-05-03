// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "GraphPrinterEditorExtension/UIExtensions/ToolbarExtender.h"
#include "GraphPrinterEditorExtension/UIExtensions/GraphPrinterStyle.h"

namespace GraphPrinterEditorExtension
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
		// Register command actions.
		FGraphPrinterCommands::Register();
		FGraphPrinterCommands::Bind();

		// Register style set.
		FGraphPrinterStyle::Register();
		
		// Register toolbar extension.
		FToolbarExtender::Register();
	}

	void FGraphPrinterEditorExtensionModule::ShutdownModule()
	{
		// Unregister toolbar extension.
		FToolbarExtender::Unregister();

		// Unregister style set.
		FGraphPrinterStyle::Unregister();
		
		// Unregister command actions.
		FGraphPrinterCommands::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinterEditorExtension::FGraphPrinterEditorExtensionModule, GraphPrinterEditorExtension)
