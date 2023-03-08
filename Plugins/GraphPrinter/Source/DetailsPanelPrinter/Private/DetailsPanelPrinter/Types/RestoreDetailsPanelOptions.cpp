// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Types/RestoreDetailsPanelOptions.h"

URestoreDetailsPanelOptions::URestoreDetailsPanelOptions()
	: bWhetherToAlsoRestoreExpandedStates(true)
{
}

URestoreWidgetOptions* URestoreDetailsPanelOptions::Duplicate(const TSubclassOf<URestoreWidgetOptions>& DestinationClass) const
{
	auto* Destination = Super::Duplicate(DestinationClass);
	if (auto* CastedDestination = Cast<URestoreDetailsPanelOptions>(Destination))
	{
		CastedDestination->bWhetherToAlsoRestoreExpandedStates = bWhetherToAlsoRestoreExpandedStates;
	}
	
	return Destination;
}
