// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterEditorExtensionSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphPrinterEditorExtensionSettings)
#endif

UGraphPrinterEditorExtensionSettings::UGraphPrinterEditorExtensionSettings()
	: bShowSubMenuInToolMenu(true)
	, bShowComboButtonInToolbar(true)
	, bShowComboButtonInStatusBar(true)
	, bCollectTargetWidgetsAutomatically(false)
{
}

bool UGraphPrinterEditorExtensionSettings::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEditChange = true;
	if (InProperty != nullptr)
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterEditorExtensionSettings, bShowComboButtonInStatusBar))
		{
#if !UE_5_00_OR_LATER
			bCanEditChange = false;
#endif
		}
	}
	
	return (Super::CanEditChange(InProperty) && bCanEditChange);
}

FString UGraphPrinterEditorExtensionSettings::GetSettingsName() const
{
	return TEXT("EditorExtension");
}
