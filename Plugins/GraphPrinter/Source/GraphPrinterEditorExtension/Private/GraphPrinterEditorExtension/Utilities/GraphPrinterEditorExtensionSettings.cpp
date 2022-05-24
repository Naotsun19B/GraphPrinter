// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"

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

UGraphPrinterSettings::FSettingsInfo UGraphPrinterEditorExtensionSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("EditorExtension"));
}
