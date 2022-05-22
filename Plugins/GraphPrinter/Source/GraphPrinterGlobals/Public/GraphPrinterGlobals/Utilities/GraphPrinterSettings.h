// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSubSettings.h"
#include "GraphPrinterSettings.generated.h"

/**
 * Editor settings for this plugin.
 */
UCLASS()
class GRAPHPRINTERGLOBALS_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()

public:
	// Constructor.
	UGraphPrinterSettings();
	
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Open the settings menu for this plugin.
	static void OpenSettings();

	// Partial friend for automatic registration of sub setting class.
	struct FSubSettingsRegister
	{
	private:
		friend class UGraphPrinterSubSettings;
		static void Register(UClass* Class);
	};
	
protected:
	// Setting classes added from other modules.
	UPROPERTY(EditAnywhere)
	FGraphPrinterSubSettingsArray SubSettingsArray;
};
