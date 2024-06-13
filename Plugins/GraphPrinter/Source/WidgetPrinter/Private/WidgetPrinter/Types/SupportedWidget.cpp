// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/SupportedWidget.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "Framework/Application/SlateApplication.h"

namespace GraphPrinter
{
	FSupportedWidget::FSupportedWidget(const TSharedRef<SWidget>& InWidget, const FString& InDisplayName, const int32 InPriority)
		: Identifier(FGuid::NewGuid())
		, Widget(InWidget)
		, DisplayName(FText::FromString(InDisplayName))
		, Priority(InPriority)
	{
	}

	bool FSupportedWidget::IsValid() const
	{
		if (!Widget.IsValid())
		{
			return false;
		}
		
		auto& SlateApplication = FSlateApplication::Get();
				
		FWidgetPath WidgetPath;
		if (!SlateApplication.FindPathToWidget(Widget.Pin().ToSharedRef(), WidgetPath))
		{
			return false;
		}
		
		return WidgetPath.IsValid();
	}

	const FGuid& FSupportedWidget::GetIdentifier() const
	{
		return Identifier;
	}

	TSharedPtr<SWidget> FSupportedWidget::GetWidget() const
	{
		return Widget.Pin();
	}

	const FText& FSupportedWidget::GetDisplayName() const
	{
		return DisplayName;
	}

	bool FSupportedWidget::operator==(const FSupportedWidget& Other) const
	{
		if (Identifier == Other.Identifier)
		{
			return true;	
		}
		
		if (!Widget.IsValid() || !Other.Widget.IsValid())
		{
			return (!Widget.IsValid() && !Other.Widget.IsValid());
		}
		
		return (Widget.Pin()->GetId() == Other.Widget.Pin()->GetId());
	}

	bool FSupportedWidget::operator<(const FSupportedWidget& Other) const
	{
		return (Priority < Other.Priority);
	}
}
