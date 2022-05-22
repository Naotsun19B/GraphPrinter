// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"

#define LOCTEXT_NAMESPACE "GraphPrinterEditorExtensionSettings"

UGraphPrinterEditorExtensionSettings::UGraphPrinterEditorExtensionSettings()
	: bHideToolbarComboButton(false)
{
}

const UGraphPrinterEditorExtensionSettings& UGraphPrinterEditorExtensionSettings::Get()
{
	const auto* Settings = GetDefault<UGraphPrinterEditorExtensionSettings>();
	check(IsValid(Settings));
	return *Settings;
}

FText UGraphPrinterEditorExtensionSettings::GetDetailsTitle() const
{
	return LOCTEXT("DetailsTitle", "Editor Extension");
}

#undef LOCTEXT_NAMESPACE
