// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Types/RestoreDetailsPanelOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(RestoreDetailsPanelOptions)
#endif

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
