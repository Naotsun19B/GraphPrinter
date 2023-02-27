// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"

UPrintDetailsPanelOptions::UPrintDetailsPanelOptions()
	: bExpandHierarchyWhenPrinting(true)
{
}

UPrintWidgetOptions* UPrintDetailsPanelOptions::Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const
{
	auto* Destination = Super::Duplicate(DestinationClass);
	if (auto* CastedDestination = Cast<UPrintDetailsPanelOptions>(Destination))
	{
		CastedDestination->bExpandHierarchyWhenPrinting = bExpandHierarchyWhenPrinting;
	}
	
	return Destination;
}
