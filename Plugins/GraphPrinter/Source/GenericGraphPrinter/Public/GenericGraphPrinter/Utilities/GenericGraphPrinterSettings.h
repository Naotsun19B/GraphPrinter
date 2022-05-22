// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "GenericGraphPrinterSettings.generated.h"

/**
 * A class that sets the default values for UPrintGraphOptions from the editor preferences.
 */
UCLASS()
class UGenericGraphPrinterSettings : public UGraphPrinterSubSettings
{
	GENERATED_BODY()

public:
	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bDrawOnlyGraph;

public:
	// Constructor.
	UGenericGraphPrinterSettings();

	// Returns reference of this settings.
	static const UGenericGraphPrinterSettings& Get();

	// UGraphPrinterSubSettings interface.
	virtual FText GetDetailsTitle() const override;
	// End of UGraphPrinterSubSettings interface.
};
