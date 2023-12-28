// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"

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
	}

	void FGraphPrinterEditorExtensionModule::ShutdownModule()
	{
		// Unregister command actions.
		FGraphPrinterCommands::Unregister();

		// Unregister style set.
		FGraphPrinterStyle::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterEditorExtensionModule, GraphPrinterEditorExtension)
