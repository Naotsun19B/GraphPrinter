// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

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
	}
}

#define GP_CAST_SLATE_WIDGET(ToClass, FromPtr) GraphPrinter::Private::CastSlateWidget<ToClass>(FromPtr, #ToClass)
