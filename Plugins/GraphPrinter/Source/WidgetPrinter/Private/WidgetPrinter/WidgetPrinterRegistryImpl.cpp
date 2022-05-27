// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IMainFrameModule.h"
#include "UObject/Class.h"
#include "UObject/UObjectIterator.h"

#if BEFORE_UE_4_27
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
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(UPrintWidgetOptions*  Options) const override;
		virtual UWidgetPrinter* FindAvailableWidgetPrinter(URestoreWidgetOptions* Options) const override;
		// End of IWidgetPrinterRegistry interface.
	
	private:
		// Called when the editor mainframe has been created.
		void HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);
		
		// Called when the hot reload is complete.
#if BEFORE_UE_4_27
		void HandleOnHotReload(bool bWasTriggeredAutomatically);
#else
		void HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason);
#endif
		
		// Collect instances of inherited classes of all existing UWidgetPrinter class.
		void CollectWidgetPrinters();
		
	private:
		// All existing classes that inherits from UWidgetPrinters.
		TArray<TSubclassOf<UWidgetPrinter>> WidgetPrinterClasses;
	};
	
	FWidgetPrinterRegistryImpl::FWidgetPrinterRegistryImpl()
	{
		// Collect widget printers and try to recollect them at hot reload.
		IMainFrameModule::Get().OnMainFrameCreationFinished().AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnMainFrameCreationFinished);
		
#if BEFORE_UE_4_27
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnHotReload);
#else
		FCoreUObjectDelegates::ReloadCompleteDelegate.AddRaw(this, &FWidgetPrinterRegistryImpl::HandleOnReloadComplete);
#endif
	}

	FWidgetPrinterRegistryImpl::~FWidgetPrinterRegistryImpl()
	{
#if BEFORE_UE_4_27
		IHotReloadInterface& HotReloadInterface = FModuleManager::LoadModuleChecked<IHotReloadInterface>(TEXT("HotReload"));
		HotReloadInterface.OnHotReload().RemoveAll(this);
#else
		FCoreUObjectDelegates::ReloadCompleteDelegate.RemoveAll(this);
#endif
		
		WidgetPrinterClasses.Reset();
	}
	
	UWidgetPrinter* FWidgetPrinterRegistryImpl::FindAvailableWidgetPrinter(UPrintWidgetOptions* Options) const
	{
		for (const auto& WidgetPrinterClass : WidgetPrinterClasses)
		{
			if (!IsValid(WidgetPrinterClass))
			{
				continue;
			}

			if (auto* WidgetPrinter = NewObject<UWidgetPrinter>(GetTransientPackage(), WidgetPrinterClass))
			{
				if (WidgetPrinter->CanPrintWidget(Options))
				{
					return WidgetPrinter;
				}
			}
		}

		return nullptr;
	}
	
	UWidgetPrinter* FWidgetPrinterRegistryImpl::FindAvailableWidgetPrinter(URestoreWidgetOptions* Options) const
	{
		for (const auto& WidgetPrinterClass : WidgetPrinterClasses)
		{
			if (!IsValid(WidgetPrinterClass))
			{
				continue;
			}

			if (auto* WidgetPrinter = NewObject<UWidgetPrinter>(GetTransientPackage(), WidgetPrinterClass))
			{
				if (WidgetPrinter->CanRestoreWidget(Options))
				{
					return WidgetPrinter;
				}
			}
		}

		return nullptr;
	}

	void FWidgetPrinterRegistryImpl::HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
	{
		CollectWidgetPrinters();

		IMainFrameModule::Get().OnMainFrameCreationFinished().RemoveAll(this);
	}
	
#if BEFORE_UE_4_27
	void FWidgetPrinterRegistryImpl::HandleOnHotReload(bool bWasTriggeredAutomatically)
#else
	void FWidgetPrinterRegistryImpl::HandleOnReloadComplete(EReloadCompleteReason ReloadCompleteReason)
#endif
	{
		CollectWidgetPrinters();
	}

	void FWidgetPrinterRegistryImpl::CollectWidgetPrinters()
	{
		WidgetPrinterClasses.Reset();

		for (auto* Class : TObjectRange<UClass>())
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

			WidgetPrinterClasses.Add(Class);
		}

		WidgetPrinterClasses.Sort(
			[](const TSubclassOf<UWidgetPrinter>& Lhs, const TSubclassOf<UWidgetPrinter>& Rhs) -> bool
			{
				return (UWidgetPrinter::GetPriority(Lhs) > UWidgetPrinter::GetPriority(Rhs));
			}
		);
		
		UE_LOG(LogGraphPrinter, Log, TEXT("---------- Registered Widget Printer Classes ----------"));
		for (const auto& WidgetPrinterClass : WidgetPrinterClasses)
		{
			if (IsValid(WidgetPrinterClass))
			{
				UE_LOG(LogGraphPrinter, Log, TEXT("%s : Priority = %d"), *GetNameSafe(WidgetPrinterClass), UWidgetPrinter::GetPriority(WidgetPrinterClass));
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
