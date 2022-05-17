// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/IWidgetPrinterRegistry.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IMainFrameModule.h"
#include "UObject/Class.h"
#include "UObject/UObjectIterator.h"

#if !BEFORE_UE_5_00
#include "Misc/HotReloadInterface.h"
#endif

namespace GraphPrinter
{
	class FWidgetPrinterRegistryImpl : public IWidgetPrinterRegistry
	{
	public:
		// Constructor.
		FWidgetPrinterRegistryImpl();
		
		// Destructor.
		virtual ~FWidgetPrinterRegistryImpl() override;
		
		// IWidgetPrinterRegistry interface.
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(const FPrintWidgetOptions& Options) const override;
#ifdef WITH_TEXT_CHUNK_HELPER
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(const FRestoreWidgetOptions& Options) const override;
#endif
		// End of IWidgetPrinterRegistry interface.
	
	private:
		// Called when the editor mainframe has been created.
		void HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);
		
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
	
	FWidgetPrinterRegistryImpl::FWidgetPrinterRegistryImpl()
	{
		// Collect widget printers and try to recollect them at hot reload.
		IMainFrameModule::Get().OnMainFrameCreationFinished().AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnMainFrameCreationFinished);
		
#if BEFORE_UE_5_00
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnReloadComplete);
#else
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnHotReload);
#endif
	}

	FWidgetPrinterRegistryImpl::~FWidgetPrinterRegistryImpl()
	{
#if BEFORE_UE_5_00
		FCoreUObjectDelegates::ReloadCompleteDelegate.RemoveAll(this);
#else
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().RemoveAll(this);
#endif
		
		WidgetPrinters.Reset();
	}

	UWidgetPrinter* FWidgetPrinterRegistryImpl::FindAvailableWidgetPrinter(const FPrintWidgetOptions& Options) const
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
	UWidgetPrinter* FWidgetPrinterRegistryImpl::FindAvailableWidgetPrinter(const FRestoreWidgetOptions& Options) const
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

	void FWidgetPrinterRegistryImpl::HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
	{
		CollectWidgetPrinters();

		IMainFrameModule::Get().OnMainFrameCreationFinished().RemoveAll(this);
	}
	
#if BEFORE_UE_5_00
	void FWidgetPrinterRegistryImpl::HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason)
#else
	void FWidgetPrinterRegistryImpl::HandleOnHotReload(bool bWasTriggeredAutomatically)
#endif
	{
		CollectWidgetPrinters();
	}

	void FWidgetPrinterRegistryImpl::CollectWidgetPrinters()
	{
		WidgetPrinters.Reset();

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

		// #TODO: If you create a material graph or reference viewer printer in another module,
		// #TODO: it will access an unintended function and crash, so verification is required.
		
		WidgetPrinters.Sort(
			[](const UWidgetPrinter& Lhs, const UWidgetPrinter& Rhs) -> bool
			{
				return (Lhs.GetPriority() > Rhs.GetPriority());
			}
		);
		
		UE_LOG(LogGraphPrinter, Log, TEXT("---------- Registered Widget Printer Classes ----------"));
		for (const auto& WidgetPrinter : WidgetPrinters)
		{
			if (IsValid(WidgetPrinter))
			{
				UE_LOG(LogGraphPrinter, Log, TEXT("%s : Priority = %d"), *GetNameSafe(WidgetPrinter->GetClass()), WidgetPrinter->GetPriority());
			}
		}
		UE_LOG(LogGraphPrinter, Log, TEXT("-------------------------------------------------------"));
	}

	namespace WidgetPrinterRegistry
	{
		static TUniquePtr<FWidgetPrinterRegistryImpl> Instance;
	}
	
	void IWidgetPrinterRegistry::Register()
	{
		WidgetPrinterRegistry::Instance = MakeUnique<FWidgetPrinterRegistryImpl>();
	}

	void IWidgetPrinterRegistry::Unregister()
	{
		WidgetPrinterRegistry::Instance.Reset();
	}

	IWidgetPrinterRegistry& IWidgetPrinterRegistry::Get()
	{
		// Don't call before Register is called or after Unregister is called.
		check(WidgetPrinterRegistry::Instance.IsValid());
		return *WidgetPrinterRegistry::Instance.Get();
	}
}
