﻿// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSettings.generated.h"

enum TextureFilter;
enum class EDesiredImageFormat : uint8;

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor)
class WIDGETPRINTER_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the only output image format is png.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (EditCondition = "bWithTextChunkHelper", HideEditConditionToggle))
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

	// Set the maximum size of the output image. There is no limit at (0.f, 0.f).
	// If set it too high, you may run out of video memory and crash.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	FVector2D MaxImageSize;

	// If the scale for drawing the graph is 0.5, it will be drawn at half the resolution.
	// Decrease the value if you want to draw a graph larger than MaxImageSize.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (ClampMin = 0.1f, ClampMax = 1.f, UIMin = 0.1f, UIMax = 1.f))
	float RenderingScale;

	// Whether to overwrite the image if it already exists when export.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	bool bCanOverwriteFileWhenExport;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	FDirectoryPath OutputDirectory;	

protected:
	// An internal variable used to determine if bIsIncludeNodeInfoInImageFile can be edited.
	UPROPERTY()
	bool bWithTextChunkHelper;
	
public:
	// Constructor.
	UGraphPrinterSettings();
	
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static const UGraphPrinterSettings& Get();

	// Open the settings menu for this plugin.
	static void OpenSettings();

protected:
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.

	// Functions called when the value changes.
	void ModifyFormat();
	void ModifyCompressionQuality();
	void ModifyMaxImageSize();
};