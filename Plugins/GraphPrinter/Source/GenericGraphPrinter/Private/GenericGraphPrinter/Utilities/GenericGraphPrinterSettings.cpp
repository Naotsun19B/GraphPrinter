// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/Utilities/GenericGraphPrinterSettings.h"

#define LOCTEXT_NAMESPACE "GenericGraphPrinterSettings"

UGenericGraphPrinterSettings::UGenericGraphPrinterSettings()
	: bDrawOnlyGraph(false)
{
}

const UGenericGraphPrinterSettings& UGenericGraphPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UGenericGraphPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

FText UGenericGraphPrinterSettings::GetDetailsTitle() const
{
	return LOCTEXT("DetailsTitle", "Generic Graph Printer");
}

#undef LOCTEXT_NAMESPACE
