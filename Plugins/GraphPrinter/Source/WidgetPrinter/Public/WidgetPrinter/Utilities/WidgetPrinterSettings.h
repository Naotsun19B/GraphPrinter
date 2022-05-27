// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "Engine/EngineTypes.h"
#include "WidgetPrinterSettings.generated.h"

enum TextureFilter;
enum class EDesiredImageFormat : uint8;

/**
 * A class that sets the default values for UPrintWidgetOptions from the editor preferences.
 */
UCLASS()
class WIDGETPRINTER_API UWidgetPrinterSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()
	
public:
	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the image formats that can be selected are limited.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (EditCondition = "bWithTextChunkHelper", HideEditConditionToggle))
	bool bIsIncludeNodeInfoInImageFile;

	// The desired output image format to write to disk.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	EDesiredImageFormat Format;

	// An image format specific compression setting. Either 0 (Default) or 1 (Uncompressed) for EXRs, or a value between 0 and 100.
	UPROPERTY(EditAnywhere, Config, Category = "Image", meta = (UIMin = 0, UIMax = 100))
	int32 CompressionQuality;

	// Texture filtering mode to use when outputting.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	TEnumAsByte<TextureFilter> FilteringMode;
	
	// Whether to output the drawing result affected by the gamma value.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bUseGamma;

	// Set the maximum size of the output image. There is no limit at (0.f, 0.f).
	// If set it too high, you may run out of video memory and crash.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	FVector2D MaxImageSize;

	// If the scale for drawing the graph is 0.5, it will be drawn at half the resolution.
	// Decrease the value if you want to draw a graph larger than MaxImageSize.
	// Decreasing the value may cause defects in the drawing result.
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
	UWidgetPrinterSettings();
	
	// Returns reference of this settings.
	static const UWidgetPrinterSettings& Get();
	
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.

	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.

protected:
	// Functions called when the value changes.
	void ModifyFormat();
	void ModifyCompressionQuality();
	void ModifyMaxImageSize();
};
