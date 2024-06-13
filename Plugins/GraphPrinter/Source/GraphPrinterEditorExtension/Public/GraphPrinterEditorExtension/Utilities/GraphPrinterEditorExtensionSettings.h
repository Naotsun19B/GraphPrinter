// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterEditorExtensionSettings.generated.h"

/**
 * A editor preferences class related to the editor extension of this plugin.
 */
UCLASS()
class UGraphPrinterEditorExtensionSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// Whether to display submenus that perform plugin functions in the editor's tools menu.
	UPROPERTY(EditAnywhere, Config, Category = "UI", meta = (ConfigRestartRequired = true))
	bool bShowSubMenuInToolMenu;
	
	// Whether to show the combo button that performs the function of the plugin in the toolbar of the asset editor.
	UPROPERTY(EditAnywhere, Config, Category = "UI", meta = (ConfigRestartRequired = true))
	bool bShowComboButtonInToolbar;

	// Whether to show the combo button that performs the function of the plugin in the editor's status bar.
	UPROPERTY(EditAnywhere, Config, Category = "UI", meta = (ConfigRestartRequired = true))
	bool bShowComboButtonInStatusBar;

	// Whether to automatically collect the target widgets when opening any type of menu.
	UPROPERTY(EditAnywhere, Config, Category = "UI")
	bool bCollectTargetWidgetsAutomatically;

public:
	// Constructor.
	UGraphPrinterEditorExtensionSettings();

	// Returns reference of this settings.
	static const UGraphPrinterEditorExtensionSettings& Get();
	
	// UObject interface.
    virtual bool CanEditChange(const FProperty* InProperty) const override;
    // End of UObject interface.

	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
