// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Types/PrintMaterialGraphOptions.h"

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
