// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterSettings.h"

UDetailsPanelPrinterSettings::UDetailsPanelPrinterSettings()
	: Padding(50.f)
	, bIsIncludeExpansionStateInImageFile(true)
	, bWhetherToAlsoRestoreExpandedStates(true)
{
}

const UDetailsPanelPrinterSettings& UDetailsPanelPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UDetailsPanelPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

UGraphPrinterSettings::FSettingsInfo UDetailsPanelPrinterSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("DetailsPanelPrinter"));
}
