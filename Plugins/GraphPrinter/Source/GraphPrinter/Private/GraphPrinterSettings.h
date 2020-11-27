// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSettings.generated.h"

class ISettingsModule;
enum class EDesiredImageFormat : uint8;

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor, DefaultConfig)
class UGraphPrinterSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
protected:
	// Directory name where the image file is output.
	static const FString OutputDirectoryName;

public:
	// The desired output image format to write to disk.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	EDesiredImageFormat Format;

	// An image format specific compression setting. Either 0 (Default) or 1 (Uncompressed) for EXRs, or a value between 0 and 100.
	UPROPERTY(Config, EditAnywhere, Category = "Image", meta = (UIMin = 0, UIMax = 100))
	int32 CompressionQuality;

	// Whether to overwrite the image if it already exists when export.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	bool bCanOverwriteFileWhenExport;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	FString OutputDirectoryPath;

public:
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();

protected:
	// Get SettingsModule.
	static ISettingsModule* GetSettingsModule();

	// UObject interface.
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
};
