// Copyright 2020-2024 Naotsun. All Rights Reserved.

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
		// Registers style set.
		FGraphPrinterStyle::Register();
		
		// Registers command actions.
		FGraphPrinterCommands::Register();
		FGraphPrinterCommands::Bind();
	}

	void FGraphPrinterEditorExtensionModule::ShutdownModule()
	{
		// Unregisters command actions.
		FGraphPrinterCommands::Unregister();

		// Unregisters style set.
		FGraphPrinterStyle::Unregister();
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterEditorExtensionModule, GraphPrinterEditorExtension)
