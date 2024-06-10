// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Types/PrintMaterialGraphOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PrintMaterialGraphOptions)
#endif

UPrintMaterialGraphOptions::UPrintMaterialGraphOptions()
	: MaterialGraphExportMethod(EMaterialGraphExportMethod::CombinePreviewAndGraph)
{
}

UPrintWidgetOptions* UPrintMaterialGraphOptions::Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const
{
	auto* Destination = Super::Duplicate(DestinationClass);
	if (auto* CastedDestination = Cast<UPrintMaterialGraphOptions>(Destination))
	{
		CastedDestination->MaterialGraphExportMethod = MaterialGraphExportMethod;
	}
	
	return Destination;
}
