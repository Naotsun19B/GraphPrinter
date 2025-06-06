// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/Utilities/MaterialGraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(MaterialGraphPrinterSettings)
#endif

UMaterialGraphPrinterSettings::UMaterialGraphPrinterSettings()
	: MaterialGraphExportMethod(EMaterialGraphExportMethod::CombinePreviewAndGraph)
{
}

FString UMaterialGraphPrinterSettings::GetSettingsName() const
{
	return TEXT("MaterialGraphPrinter");
}
