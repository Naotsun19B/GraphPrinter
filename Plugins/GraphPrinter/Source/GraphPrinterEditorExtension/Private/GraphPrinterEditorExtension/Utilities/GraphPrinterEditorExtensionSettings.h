// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterEditorExtensionSettings.generated.h"

/**
 * Settings related to the editor extension of this plugin.
 */
UCLASS()
class UGraphPrinterEditorExtensionSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// Whether to hide the combo button that performs the function of the plugin in the toolbar of the asset editor.
	UPROPERTY(EditAnywhere, Config, Category = "UI", meta = (ConfigRestartRequired = true))
	bool bHideToolbarComboButton;

public:
	// Constructor.
	UGraphPrinterEditorExtensionSettings();

	// Returns reference of this settings.
	static const UGraphPrinterEditorExtensionSettings& Get();
	
	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
