// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Utilities/GenericGraphPrinterSettings.h"

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
