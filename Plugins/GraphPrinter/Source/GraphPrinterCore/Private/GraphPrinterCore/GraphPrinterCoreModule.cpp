// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/IGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/WidgetPrinter/WidgetPrinter.h"

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
#ifdef WITH_TEXT_CHUNK_HELPER
		virtual void RestoreWidget(const FRestoreWidgetOptions& Options) override;
#endif
		// End of IGraphPrinter interface.
	};

	void FGraphPrinterCoreModule::StartupModule()
	{
		// Register settings.
		UGraphPrinterSettings::Register();
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}

	void FGraphPrinterCoreModule::PrintWidget(const FPrintWidgetOptions& Options)
	{
		const TArray<UWidgetPrinter*>& WidgetPrinters = UGraphPrinterSettings::Get().GetWidgetPrinters();
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				if (WidgetPrinter->CanPrintWidget(Options))
				{
					WidgetPrinter->PrintWidget(Options);
					break;
				}
			}
		}
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCoreModule::RestoreWidget(const FRestoreWidgetOptions& Options)
	{
		const TArray<UWidgetPrinter*>& WidgetPrinters = UGraphPrinterSettings::Get().GetWidgetPrinters();
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				if (WidgetPrinter->CanRestoreWidget(Options))
				{
					WidgetPrinter->RestoreWidget(Options);
					break;
				}
			}
		}
	}
#endif
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterCoreModule, GraphPrinter)
