// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DetailsPanelPrinterSettings)
#endif

UDetailsPanelPrinterSettings::UDetailsPanelPrinterSettings()
	: Padding(50.f)
	, bIsIncludeExpansionStateInImageFile(true)
	, bWhetherToAlsoRestoreExpandedStates(true)
{
#if !UE_5_00_OR_LATER
	if (UClass* Class = GetClass())
	{
		Class->ClassFlags |= CLASS_Abstract;
	}
#endif
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
