// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "MaterialGraphPrinter/Types/MaterialGraphExportMethod.h"
#include "PrintMaterialGraphOptions.generated.h"

/**
 * An optional class to specify when printing the material graph editor.
 */
UCLASS()
class UPrintMaterialGraphOptions : public UPrintGraphOptions
{
	GENERATED_BODY()

public:
	// Constructor.
	UPrintMaterialGraphOptions();

	// UPrintWidgetOptions interface.
	virtual UPrintWidgetOptions* Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const override;
	// End of UPrintWidgetOptions interface.

public:
	// How to output a graph in the material editor.
	EMaterialGraphExportMethod MaterialGraphExportMethod;
};
