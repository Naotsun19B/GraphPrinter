// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/IGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/WidgetPrinter/WidgetPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if !BEFORE_UE_5_00
#include "Misc/HotReloadInterface.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPrinterCoreModule"

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

	private:
		// Execution confirmation function used internally.
		UWidgetPrinter* FindAvailableWidgetPrinter(const FPrintWidgetOptions& Options) const;
#ifdef WITH_TEXT_CHUNK_HELPER
		UWidgetPrinter* FindAvailableWidgetPrinter(const FRestoreWidgetOptions& Options) const;
#endif
		
		// Called when the hot reload is complete.
#if BEFORE_UE_5_00
		void HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason);
#else
		void HandleOnHotReload(bool bWasTriggeredAutomatically);
#endif
		
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
#if BEFORE_UE_5_00
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw(this, &FGraphPrinterCoreModule::HandleOnReloadComplete);
#else
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().AddRaw(this, &FGraphPrinterCoreModule::HandleOnHotReload);
#endif
	}

	void FGraphPrinterCoreModule::ShutdownModule()
	{
#if BEFORE_UE_5_00
		FCoreUObjectDelegates::ReloadCompleteDelegate.RemoveAll(this);
#else
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().RemoveAll(this);
#endif
		WidgetPrinters.Reset();
		
		// Unregister settings.
		UGraphPrinterSettings::Unregister();
	}

	void FGraphPrinterCoreModule::PrintWidget(const FPrintWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->PrintWidget(Options);
		}
	}

	bool FGraphPrinterCoreModule::CanPrintWidget(const FPrintWidgetOptions& Options)
	{
		return IsValid(FindAvailableWidgetPrinter(Options));
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCoreModule::RestoreWidget(const FRestoreWidgetOptions& Options)
	{
		if (UWidgetPrinter* FoundWidgetPrinter = FindAvailableWidgetPrinter(Options))
		{
			FoundWidgetPrinter->RestoreWidget(Options);
		}
	}

	bool FGraphPrinterCoreModule::CanRestoreWidget(const FRestoreWidgetOptions& Options)
	{
		return IsValid(FindAvailableWidgetPrinter(Options));
	}
#endif

	UWidgetPrinter* FGraphPrinterCoreModule::FindAvailableWidgetPrinter(const FPrintWidgetOptions& Options) const
	{
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				if (WidgetPrinter->CanPrintWidget(Options))
				{
					return WidgetPrinter;
				}
			}
		}

		return nullptr;
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	UWidgetPrinter* FGraphPrinterCoreModule::FindAvailableWidgetPrinter(const FRestoreWidgetOptions& Options) const
	{
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				if (WidgetPrinter->CanRestoreWidget(Options))
				{
					return WidgetPrinter;
				}
			}
		}

		return nullptr;
	}
#endif

#if BEFORE_UE_5_00
	void FGraphPrinterCoreModule::HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason)
#else
	void FGraphPrinterCoreModule::HandleOnHotReload(bool bWasTriggeredAutomatically)
#endif
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(GraphPrinter::FGraphPrinterCoreModule, GraphPrinter)
