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

	private:
		// Called when the hot reload is complete.
		void HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason);
		
		// Collect instances of inherited classes of all existing UWidgetPrinter class.
		void CollectWidgetPrinters();
		
	private:
		// Instances of inherited classes of all existing UWidgetPrinter class.
		TArray<UWidgetPrinter*> WidgetPrinters;
	};
	
	void FGraphPrinterCoreModule::StartupModule()
	{
		// Register settings.
		UGraphPrinterSettings::Register();

		// Collect widget printers and try to recollect them at hot reload.
		CollectWidgetPrinters();
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw(this, &FGraphPrinterCoreModule::HandleOnReloadComplete);
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
		FCoreUObjectDelegates::ReloadCompleteDelegate.RemoveAll(this);
		WidgetPrinters.Reset();
		
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}

	void FGraphPrinterCoreModule::PrintWidget(const FPrintWidgetOptions& Options)
	{
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

	void FGraphPrinterCoreModule::HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason)
	{
		CollectWidgetPrinters();
	}

	void FGraphPrinterCoreModule::CollectWidgetPrinters()
	{
		for (const auto* Class : TObjectRange<UClass>())
		{
			if (!IsValid(Class))
			{
				continue;
			}
			
			if (!Class->IsChildOf(UWidgetPrinter::StaticClass()))
			{
				continue;
			}
			
			if (Class->HasAnyClassFlags(CLASS_Abstract))
			{
				continue;
			}
			
			if (auto* WidgetPrinter = Cast<UWidgetPrinter>(Class->GetDefaultObject()))
			{
				WidgetPrinters.Add(WidgetPrinter);
			}
		}
		
		TArray<UWidgetPrinter*> NoElementDuplicate;
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (!NoElementDuplicate.Contains(WidgetPrinter))
			{
				NoElementDuplicate.Add(WidgetPrinter);
			}
		}
		WidgetPrinters = NoElementDuplicate;
	
		WidgetPrinters.Sort(
			[](const UWidgetPrinter& Lhs, const UWidgetPrinter& Rhs) -> bool
			{
				return (Lhs.GetPriority() < Rhs.GetPriority());
			}
		);
	}
}

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterCoreModule, GraphPrinter)
