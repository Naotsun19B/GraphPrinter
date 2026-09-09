// Copyright 2020-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"

namespace GraphPrinter
{
	namespace Private
	{
		/**
		 * Cast function for classes that inherit from SWidget.
		 */
		template<class To, class From>
		TSharedPtr<To> CastSlateWidget(TSharedPtr<From> FromPtr, const FName& ToClassName)
		{
			static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
			static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

			if (FromPtr.IsValid())
			{
				if (FromPtr->GetType() == ToClassName)
				{
					return StaticCastSharedPtr<To>(FromPtr);
				}
			}

			return nullptr;
		}

		/**
		 * Finds a widget of the specified class from the widget itself or its ancestors.
		 * Never matches a widget that belongs to an unrelated sibling tab.
		 * Takes the widget by value because it is advanced while walking up the parent path.
		 */
		template<class To>
		TSharedPtr<To> FindSlateWidgetInParentPath(TSharedPtr<SWidget> Widget, const FName& ToClassName)
		{
			while (Widget.IsValid())
			{
				const TSharedPtr<To> FoundWidget = CastSlateWidget<To>(Widget, ToClassName);
				if (FoundWidget.IsValid())
				{
					return FoundWidget;
				}

				Widget = Widget->GetParentWidget();
			}

			return nullptr;
		}
	}
}

#define GP_CAST_SLATE_WIDGET(ToClass, FromPtr) GraphPrinter::Private::CastSlateWidget<ToClass>(FromPtr, #ToClass)
#define GP_FIND_SLATE_WIDGET_IN_PARENT_PATH(ToClass, FromPtr) GraphPrinter::Private::FindSlateWidgetInParentPath<ToClass>(FromPtr, #ToClass)
