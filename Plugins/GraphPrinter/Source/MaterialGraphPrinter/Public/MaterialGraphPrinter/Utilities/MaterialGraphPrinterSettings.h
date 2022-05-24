// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "MaterialGraphPrinter/Types/MaterialGraphExportMethod.h"
#include "MaterialGraphPrinterSettings.generated.h"

/**
 * A class that sets the default values for UPrintMaterialGraphOptions from the editor preferences.
 */
UCLASS()
class MATERIALGRAPHPRINTER_API UMaterialGraphPrinterSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// How to output a graph in the material editor.
	UPROPERTY(EditAnywhere, Config, Category = "Material Editor")
	EMaterialGraphExportMethod MaterialGraphExportMethod;
	
public:
	// Constructor.
	UMaterialGraphPrinterSettings();

	// Returns reference of this settings.
	static const UMaterialGraphPrinterSettings& Get();
	
	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
