// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Utilities/MaterialGraphPrinterSettings.h"

#define LOCTEXT_NAMESPACE "MaterialGraphPrinterSettings"

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

FText UMaterialGraphPrinterSettings::GetDetailsTitle() const
{
	return LOCTEXT("DetailsTitle", "Material Graph");
}

#undef LOCTEXT_NAMESPACE
