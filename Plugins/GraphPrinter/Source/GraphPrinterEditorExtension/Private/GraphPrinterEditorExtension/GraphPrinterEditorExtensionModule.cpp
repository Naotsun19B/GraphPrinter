// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"

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
		// Register command actions
		FGraphPrinterCommands::Register();
		FGraphPrinterCommands::Bind();
	}

	void FGraphPrinterEditorExtensionModule::ShutdownModule()
	{
		// Unregister command actions.
		FGraphPrinterCommands::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinterEditorExtension::FGraphPrinterEditorExtensionModule, GraphPrinterEditorExtension)
