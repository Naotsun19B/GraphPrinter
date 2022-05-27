// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GenericGraphPrinterSettings.generated.h"

/**
 * A class that sets the default values for UPrintGraphOptions from the editor preferences.
 */
UCLASS()
class UGenericGraphPrinterSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// Wide margin when drawing the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (UIMin = 0.f, ClampMin = 0.f))
	float Padding;
	
	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bDrawOnlyGraph;

public:
	// Constructor.
	UGenericGraphPrinterSettings();

	// Returns reference of this settings.
	static const UGenericGraphPrinterSettings& Get();

	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
