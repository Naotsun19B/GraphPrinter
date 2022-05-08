// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/IGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/WidgetPrinter/IWidgetPrinterRegistry.h"

namespace GraphPrinter
{
	const FName IGraphPrinter::PluginModuleName = TEXT("GraphPrinterCore");
	
	class FGraphPrinterCoreModule : public IGraphPrinter
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

		// IGraphPrinter interface.
		virtual void PrintWidget(const FPrintWidgetOptions& Options) override;
		virtual bool CanPrintWidget(const FPrintWidgetOptions& Options) override;
#ifdef WITH_TEXT_CHUNK_HELPER
		virtual void RestoreWidget(const FRestoreWidgetOptions& Options) override;
		virtual bool CanRestoreWidget(const FRestoreWidgetOptions& Options) override;
#endif
		// End of IGraphPrinter interface.
	};

	void FGraphPrinterCoreModule::StartupModule()
	{
		// Register settings.
		UGraphPrinterSettings::Register();

		// Register widget printer registry.
		IWidgetPrinterRegistry::Register();
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
		// Unregister widget printer registry.
		IWidgetPrinterRegistry::Unregister();
		
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}

	void FGraphPrinterCoreModule::PrintWidget(const FPrintWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->PrintWidget(Options);
		}
	}

	bool FGraphPrinterCoreModule::CanPrintWidget(const FPrintWidgetOptions& Options)
	{
		return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCoreModule::RestoreWidget(const FRestoreWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->RestoreWidget(Options);
		}
	}

	bool FGraphPrinterCoreModule::CanRestoreWidget(const FRestoreWidgetOptions& Options)
	{
		return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
	}
#endif
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterCoreModule, GraphPrinter)
