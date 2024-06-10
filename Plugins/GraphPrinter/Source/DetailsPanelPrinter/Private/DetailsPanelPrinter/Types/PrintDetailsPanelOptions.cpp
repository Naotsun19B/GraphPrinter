// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PrintDetailsPanelOptions)
#endif

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
