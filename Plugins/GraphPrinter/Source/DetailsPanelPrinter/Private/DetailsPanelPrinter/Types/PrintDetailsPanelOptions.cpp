// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"

UPrintDetailsPanelOptions::UPrintDetailsPanelOptions()
	: Padding(50.f)
	, bIsIncludeExpansionStateInImageFile(true)
{
}

UPrintWidgetOptions* UPrintDetailsPanelOptions::Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const
{
	auto* Destination = Super::Duplicate(DestinationClass);
	if (auto* CastedDestination = Cast<UPrintDetailsPanelOptions>(Destination))
	{
		CastedDestination->Padding = Padding;
		CastedDestination->bIsIncludeExpansionStateInImageFile = bIsIncludeExpansionStateInImageFile;
	}
	
	return Destination;
}
