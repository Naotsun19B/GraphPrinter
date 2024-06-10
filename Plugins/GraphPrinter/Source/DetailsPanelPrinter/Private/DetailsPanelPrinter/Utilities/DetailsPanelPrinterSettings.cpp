// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterSettings.h"
#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
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

bool UDetailsPanelPrinterSettings::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEditChange = true;
	if (InProperty != nullptr)
	{
		if ((InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UDetailsPanelPrinterSettings, bIsIncludeExpansionStateInImageFile)) ||
			(InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UDetailsPanelPrinterSettings, bWhetherToAlsoRestoreExpandedStates)))
		{
			if (!UWidgetPrinterSettings::Get().bIsIncludeWidgetInfoInImageFile)
			{
				bCanEditChange = false;
			}
		}
	}
	
	return (Super::CanEditChange(InProperty) && bCanEditChange);
}

UGraphPrinterSettings::FSettingsInfo UDetailsPanelPrinterSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("DetailsPanelPrinter"));
}
