// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Utilities/GenericGraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GenericGraphPrinterSettings)
#endif

UGenericGraphPrinterSettings::UGenericGraphPrinterSettings()
	: Padding(100.f)
	, bDrawOnlyGraph(false)
{
}

FString UGenericGraphPrinterSettings::GetSettingsName() const
{
	return TEXT("GenericGraphPrinter");
}
