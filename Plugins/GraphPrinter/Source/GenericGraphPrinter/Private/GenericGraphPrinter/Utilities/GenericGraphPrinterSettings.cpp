// Copyright 2020-2024 Naotsun. All Rights Reserved.

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

const UGenericGraphPrinterSettings& UGenericGraphPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UGenericGraphPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

UGraphPrinterSettings::FSettingsInfo UGenericGraphPrinterSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("GenericGraphPrinter"));
}
