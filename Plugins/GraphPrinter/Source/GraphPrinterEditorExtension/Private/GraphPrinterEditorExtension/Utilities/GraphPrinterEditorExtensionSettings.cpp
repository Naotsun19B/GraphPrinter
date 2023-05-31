// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphPrinterEditorExtensionSettings)
#endif

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
