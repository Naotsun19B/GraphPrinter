// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Utilities/MaterialGraphPrinterSettings.h"

UMaterialGraphPrinterSettings::UMaterialGraphPrinterSettings()
	: MaterialGraphExportMethod(EMaterialGraphExportMethod::CombinePreviewAndGraph)
{
}

const UMaterialGraphPrinterSettings& UMaterialGraphPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UMaterialGraphPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

UGraphPrinterSettings::FSettingsInfo UMaterialGraphPrinterSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("MaterialGraphPrinter"));
}
