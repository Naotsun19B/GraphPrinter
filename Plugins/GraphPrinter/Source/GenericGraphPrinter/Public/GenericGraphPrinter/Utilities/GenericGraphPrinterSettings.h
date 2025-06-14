// Copyright 2020-2025 Naotsun. All Rights Reserved.

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
	// The wide margin when drawing the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (UIMin = 0.f, ClampMin = 0.f))
	float Padding;
	
	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bDrawOnlyGraph;

public:
	// Constructor.
	UGenericGraphPrinterSettings();

	// UGraphPrinterSettings interface.
	virtual FString GetSettingsName() const override;
	// End of UGraphPrinterSettings interface.
};
