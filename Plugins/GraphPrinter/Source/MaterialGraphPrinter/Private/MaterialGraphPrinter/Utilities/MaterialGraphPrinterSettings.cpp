// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Utilities/MaterialGraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(MaterialGraphPrinterSettings)
#endif

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
