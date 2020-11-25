// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterUtils.h"
#include "GraphPrinter.h"
#include "Widgets/SWidget.h"

namespace GraphPrinterInternals
{
	// Cast function for classes that inherit from SWidget.
	template<typename To, typename From>
	TSharedPtr<To> CastSlateWidget(TSharedPtr<From> Src, const FName& DestClassName)
	{
		static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
		static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

		if (Src.IsValid())
		{
			if (Src->GetType() == DestClassName)
			{
				return StaticCastSharedPtr<To>(Src);
			}
		}

		return nullptr;
	}
}
#define CAST_SLATE_WIDGET(To, From) GraphPrinterInternals::CastSlateWidget<To>(From, #To)

void UGraphPrinterUtils::PrintGraph()
{
	UE_LOG(LogGraphPrinter, Log, TEXT("Called UGraphPrinterUtils::PrintGraph"));
}
