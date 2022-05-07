// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/IGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/Utilities/GraphPrinterUtils.h"
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
#ifdef WITH_TEXT_CHUNK_HELPER
		virtual void RestoreWidget(const FRestoreWidgetOptions& Options) override;
#endif
		// End of IGraphPrinter interface.

	private:
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
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				if (WidgetPrinter->CanPrintWidget(Options))
				{
					WidgetPrinter->PrintWidget(Options);
					return;
				}
			}
		}

		FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotFoundWidgetToPrint", "The tab that contains the printable widget is not selected."),
			FGraphPrinterUtils::CS_Fail
		);
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
					return;
				}
			}
		}

		FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotFoundWidgetToRestore", "The tab that contains the restorable widget is not selected."),
			FGraphPrinterUtils::CS_Fail
		);
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
