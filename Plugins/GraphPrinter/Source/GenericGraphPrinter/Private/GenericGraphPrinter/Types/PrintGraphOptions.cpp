// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PrintGraphOptions)
#endif

UPrintGraphOptions::UPrintGraphOptions()
	: Super()
	, Padding(0.f)
	, bDrawOnlyGraph(false)
{
}

UPrintWidgetOptions* UPrintGraphOptions::Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const
{
	auto* Destination = Super::Duplicate(DestinationClass);
	if (auto* CastedDestination = Cast<UPrintGraphOptions>(Destination))
	{
		CastedDestination->Padding = Padding;
		CastedDestination->bDrawOnlyGraph = bDrawOnlyGraph;
	}
	
	return Destination;
}
