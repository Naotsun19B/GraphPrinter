// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/ISupportedWidgetRegistry.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"

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
		virtual TArray<TSharedRef<SWidget>> GetSupportedWidgets() const override;
		virtual TSharedPtr<SWidget> GetSelectedWidget() const override;
		virtual void SetSelectedWidget(const int32 Index) override;
		// End of ISupportedWidgetRegistry interface.

	private:
		// Called when the focused widget changes.
		void HandleOnFocusChanging(
			const FFocusEvent& FocusEvent,
			const FWeakWidgetPath& OldFocusedWidgetPath,
			const TSharedPtr<SWidget>& OldFocusedWidget,
			const FWidgetPath& NewFocusedWidgetPath,
			const TSharedPtr<SWidget>& NewFocusedWidget
		);

	private:
		// The list of widgets supported by either printer.
		TArray<TWeakPtr<SWidget>> RegisteredWidgets;

		// The window containing the last focused widget.
		TWeakPtr<SWindow> LastFocusedWindow;

		// The widget selected in a menu from RegisteredWidgets.
		TWeakPtr<SWidget> SelectedWidget;
	};

	FSupportedWidgetRegistryImpl::FSupportedWidgetRegistryImpl()
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnFocusChanging().AddRaw(this, &FSupportedWidgetRegistryImpl::HandleOnFocusChanging);
		}
	}

	FSupportedWidgetRegistryImpl::~FSupportedWidgetRegistryImpl()
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().OnFocusChanging().RemoveAll(this);
		}
	}

	TArray<TSharedRef<SWidget>> FSupportedWidgetRegistryImpl::GetSupportedWidgets() const
	{
		TArray<TSharedRef<SWidget>> SupportedWidgets;

		for (const auto& RegisteredWidget : RegisteredWidgets)
		{
			if (RegisteredWidget.IsValid())
			{
				SupportedWidgets.Add(RegisteredWidget.Pin().ToSharedRef());
			}
		}

		return SupportedWidgets;
	}

	TSharedPtr<SWidget> FSupportedWidgetRegistryImpl::GetSelectedWidget() const
	{
		return SelectedWidget.Pin();
	}

	void FSupportedWidgetRegistryImpl::SetSelectedWidget(const int32 Index)
	{
		if (RegisteredWidgets.IsValidIndex(Index))
		{
			SelectedWidget = RegisteredWidgets[Index];
		}
	}

	void FSupportedWidgetRegistryImpl::HandleOnFocusChanging(
		const FFocusEvent& FocusEvent,
		const FWeakWidgetPath& OldFocusedWidgetPath,
		const TSharedPtr<SWidget>& OldFocusedWidget,
		const FWidgetPath& NewFocusedWidgetPath,
		const TSharedPtr<SWidget>& NewFocusedWidget
	)
	{
		RegisteredWidgets.RemoveAll(
			[](const TWeakPtr<SWidget>& RegisteredWidget) -> bool
			{
				if (!RegisteredWidget.IsValid())
				{
					return true;
				}

				auto& SlateApplication = FSlateApplication::Get();
				
				FWidgetPath WidgetPath;
				if (!SlateApplication.FindPathToWidget(RegisteredWidget.Pin().ToSharedRef(), WidgetPath, EVisibility::All))
				{
					return true;
				}

				if (WidgetPath.GetWindow() != SlateApplication.GetActiveTopLevelWindow())
				{
					return true;
				}

				return !WidgetPath.IsValid();
			}
		);

		if (!NewFocusedWidgetPath.IsValid())
		{
			return;
		}
		
		TWeakPtr<SWindow> NewFocusedWindow = NewFocusedWidgetPath.GetWindow();
		if (LastFocusedWindow.IsValid() && (NewFocusedWindow == LastFocusedWindow))
		{
			return;
		}
		
		LastFocusedWindow = NewFocusedWindow;

		FWidgetPrinterUtils::EnumerateChildWidgets(
			LastFocusedWindow.Pin(),
			[&](const TSharedPtr<SWidget>& ChildWidget) -> bool
			{
				if (ChildWidget.IsValid() && IWidgetPrinterRegistry::Get().IsSupportedWidget(ChildWidget.ToSharedRef()))
				{
					auto ContainsPredicate = [&ChildWidget](const TWeakPtr<SWidget>& RegisteredWidget) -> bool
					{
						return (RegisteredWidget.IsValid() && (ChildWidget->GetId() == RegisteredWidget.Pin()->GetId()));
					};
					if (!RegisteredWidgets.ContainsByPredicate(ContainsPredicate))
					{
						RegisteredWidgets.Add(ChildWidget);
					}
				}

				return true;
			}
		);

		for (const auto& RegisteredWidget : RegisteredWidgets)
		{
			if (!RegisteredWidget.IsValid())
			{
				continue;
			}
			
			UE_LOG(LogGraphPrinter, Warning, TEXT("%s"), *RegisteredWidget.Pin()->ToString());
		}
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
