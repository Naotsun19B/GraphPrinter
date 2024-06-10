// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "DetailsPanelPrinterSettings.generated.h"

/**
 * A class that sets the default values for UPrintDetailsPanelOptions from the editor preferences.
 */
UCLASS()
class DETAILSPANELPRINTER_API UDetailsPanelPrinterSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// The height margin when drawing the details view.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (UIMin = 0.f, ClampMin = 0.f))
	float Padding;
	
	// Whether to embed the expanded state of each item in the image file and restore the expanded state of each item when restoring.
	UPROPERTY(EditAnywhere, Config, Category = "Expansion State")
	bool bIsIncludeExpansionStateInImageFile;

	// Whether to also restore the expanded state when restoring.
	UPROPERTY(EditAnywhere, Config, Category = "Expansion State")
	bool bWhetherToAlsoRestoreExpandedStates;

public:
	// Constructor.
	UDetailsPanelPrinterSettings();

	// Returns reference of this settings.
	static const UDetailsPanelPrinterSettings& Get();

	// UObject interface.
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	// End of UObject interface.
	
	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
