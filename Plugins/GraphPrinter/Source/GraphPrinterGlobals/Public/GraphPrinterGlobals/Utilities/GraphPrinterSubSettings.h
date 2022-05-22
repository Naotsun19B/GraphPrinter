// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSubSettings.generated.h"

/**
 * A sub settings class that adds configuration items from other modules.
 */
UCLASS(Abstract, Config = Editor)
class GRAPHPRINTERGLOBALS_API UGraphPrinterSubSettings : public UObject
{
	GENERATED_BODY()

public:
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
};

/**
 * A body for making editor extensions that allow multiple subsettings to be edited from the editor preferences.
 */
USTRUCT(NotBlueprintType, BlueprintInternalUseOnly)
struct FGraphPrinterSubSettingsArray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<UClass*> SubSettingsClasses;
};
