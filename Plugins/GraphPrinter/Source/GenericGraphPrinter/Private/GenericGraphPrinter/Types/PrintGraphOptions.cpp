// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Types/PrintGraphOptions.h"

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
