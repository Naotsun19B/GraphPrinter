// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/ISupportedWidgetRegistry.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "Framework/Application/SlateApplication.h"

namespace GraphPrinter
{
	class FSupportedWidgetRegistryImpl : public ISupportedWidgetRegistry
	{
	public:
		// Constructor.
		FSupportedWidgetRegistryImpl();
		
		// Destructor.
		virtual ~FSupportedWidgetRegistryImpl() override;
		
		// ISupportedWidgetRegistry interface.
		virtual void CollectSupportedWidget() override;
		virtual const TArray<FSupportedWidget>& GetSupportedWidgets() const override;
		virtual TOptional<FSupportedWidget> GetSelectedWidget() const override;
		virtual void SetSelectedWidget(const FGuid Identifier) override;
		virtual bool WasAnyMenuVisibleInPreviousFrame() const override;
		// End of ISupportedWidgetRegistry interface.

	private:
		// Called when before slate application ticks.
		void HandleOnPreTick(const float DeltaTime);

	private:
		// The list of widgets supported by either printer.
		TArray<FSupportedWidget> RegisteredWidgets;

		// The widget selected in a menu from RegisteredWidgets.
		FGuid SelectedWidgetIdentifier;

		// Whether menus such as the tools menu and toolbar were visible in the previous frame.
		bool bWasAnyMenuVisibleInPreviousFrame;

		// The lambda function that compares widget information by unique ID.
		TFunction<bool(const FSupportedWidget& RegisteredWidget)> EqualsByIdentifier = [&](const FSupportedWidget& RegisteredWidget) -> bool
		{
			return (RegisteredWidget.GetIdentifier() == SelectedWidgetIdentifier);
		};
	};

	FSupportedWidgetRegistryImpl::FSupportedWidgetRegistryImpl()
		: bWasAnyMenuVisibleInPreviousFrame(false)
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnPreTick().AddRaw(this, &FSupportedWidgetRegistryImpl::HandleOnPreTick);
		}
	}

	FSupportedWidgetRegistryImpl::~FSupportedWidgetRegistryImpl()
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnPreTick().RemoveAll(this);
		}
	}

	void FSupportedWidgetRegistryImpl::CollectSupportedWidget()
	{
		RegisteredWidgets.Empty();
		
		FWidgetPrinterUtils::EnumerateChildWidgets(
			FSlateApplication::Get().GetActiveTopLevelWindow(),
			[&](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				if (ChildWidget.IsValid())
				{
					const TOptional<FSupportedWidget>& SupportedWidget = IWidgetPrinterRegistry::Get().CheckIfSupported(ChildWidget.ToSharedRef());
					if (SupportedWidget.IsSet())
					{
						RegisteredWidgets.AddUnique(SupportedWidget.GetValue());
					}
				}

				return true;
			}
		);
	}

	const TArray<FSupportedWidget>& FSupportedWidgetRegistryImpl::GetSupportedWidgets() const
	{
		return RegisteredWidgets;
	}

	TOptional<FSupportedWidget> FSupportedWidgetRegistryImpl::GetSelectedWidget() const
	{
		if (const FSupportedWidget* FoundRegisteredWidget = RegisteredWidgets.FindByPredicate(EqualsByIdentifier))
		{
			return *FoundRegisteredWidget;
		}
		
		return {};
	}

	void FSupportedWidgetRegistryImpl::SetSelectedWidget(const FGuid Identifier)
	{
		if (RegisteredWidgets.ContainsByPredicate(EqualsByIdentifier))
		{
			SelectedWidgetIdentifier = Identifier;
		}
	}
	
	bool FSupportedWidgetRegistryImpl::WasAnyMenuVisibleInPreviousFrame() const
	{
		return bWasAnyMenuVisibleInPreviousFrame;
	}

	void FSupportedWidgetRegistryImpl::HandleOnPreTick(const float DeltaTime)
	{
		RegisteredWidgets.RemoveAll(
			[](const FSupportedWidget& RegisteredWidget) -> bool
			{
				return !RegisteredWidget.IsValid();
			}
		);

		if (!SelectedWidgetIdentifier.IsValid() || !RegisteredWidgets.ContainsByPredicate(EqualsByIdentifier))
		{
			if (!RegisteredWidgets.IsEmpty())
			{
				SelectedWidgetIdentifier = RegisteredWidgets[0].GetIdentifier();
			}
		}
		
		bWasAnyMenuVisibleInPreviousFrame = FSlateApplication::Get().AnyMenusVisible();
	}

	namespace SupportedWidgetHolder
	{
		static TUniquePtr<FSupportedWidgetRegistryImpl> Instance;
	}
	
	void ISupportedWidgetRegistry::Register()
	{
		SupportedWidgetHolder::Instance = MakeUnique<FSupportedWidgetRegistryImpl>();
	}

	void ISupportedWidgetRegistry::Unregister()
	{
		SupportedWidgetHolder::Instance.Reset();
	}

	ISupportedWidgetRegistry& ISupportedWidgetRegistry::Get()
	{
		// Don't call before Register is called or after Unregister is called.
		check(SupportedWidgetHolder::Instance.IsValid());
		return *SupportedWidgetHolder::Instance.Get();
	}
}
