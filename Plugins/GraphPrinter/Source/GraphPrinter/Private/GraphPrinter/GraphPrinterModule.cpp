// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/IGraphPrinter.h"
#include "GraphPrinter/Utilities/GraphPrinterSettings.h"
#include "GraphPrinter/WidgetPrinters/IWidgetPrinterRegistry.h"

namespace GraphPrinter
{
	const FName IGraphPrinter::PluginModuleName = TEXT("GraphPrinter");
	
	class FGraphPrinterModule : public IGraphPrinter
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

		// IGraphPrinter interface.
		virtual void PrintWidget(const FPrintWidgetOptions& Options) override;
		virtual bool CanPrintWidget(const FPrintWidgetOptions& Options) override;
		virtual void RestoreWidget(const FRestoreWidgetOptions& Options) override;
		virtual bool CanRestoreWidget(const FRestoreWidgetOptions& Options) override;
		// End of IGraphPrinter interface.
	};

	void FGraphPrinterModule::StartupModule()
	{
		// Register settings.
		UGraphPrinterSettings::Register();

		// Register widget printer registry.
		IWidgetPrinterRegistry::Register();
	}

	void FGraphPrinterModule::ShutdownModule()
	{
		// Unregister widget printer registry.
		IWidgetPrinterRegistry::Unregister();
		
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}

	void FGraphPrinterModule::PrintWidget(const FPrintWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->PrintWidget(Options);
		}
	}

	bool FGraphPrinterModule::CanPrintWidget(const FPrintWidgetOptions& Options)
	{
		return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
	}
	
	void FGraphPrinterModule::RestoreWidget(const FRestoreWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->RestoreWidget(Options);
		}
	}

	bool FGraphPrinterModule::CanRestoreWidget(const FRestoreWidgetOptions& Options)
	{
		return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterModule, GraphPrinter)
