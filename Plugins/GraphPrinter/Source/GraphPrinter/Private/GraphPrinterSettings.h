﻿// Copyright 2020 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSettings.generated.h"

enum TextureFilter;
enum class EDesiredImageFormat : uint8;

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor, DefaultConfig)
class UGraphPrinterSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the only output image format is png.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bIsIncludeNodeInfoInImageFile;

	// The desired output image format to write to disk.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (EditCondition = "!bIsIncludeNodeInfoInImageFile"))
	EDesiredImageFormat Format;

	// An image format specific compression setting. Either 0 (Default) or 1 (Uncompressed) for EXRs, or a value between 0 and 100.
	UPROPERTY(Config, EditAnywhere, Category = "Image", meta = (UIMin = 0, UIMax = 100))
	int32 CompressionQuality;

	// Texture filtering mode to use when outputting.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	TEnumAsByte<TextureFilter> FilteringMode;

	// Whether to output the drawing result affected by the gamma value.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bUseGamma;

	// Wide margin when drawing the graph editor.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (UIMin = 0.f, ClampMin = 0.f))
	float Padding;

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
	// UObject interface.
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
};