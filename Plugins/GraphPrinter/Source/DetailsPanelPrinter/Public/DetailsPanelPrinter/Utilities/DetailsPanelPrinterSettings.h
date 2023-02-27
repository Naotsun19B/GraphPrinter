// Copyright 2020-2023 Naotsun. All Rights Reserved.

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
	// Expand all hierarchies when printing the details panel.
	UPROPERTY(EditAnywhere, Config, Category = "Details Panel")
	bool bExpandHierarchyWhenPrinting;

public:
	// Constructor.
	UDetailsPanelPrinterSettings();

	// Returns reference of this settings.
	static const UDetailsPanelPrinterSettings& Get();

	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
