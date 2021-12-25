// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "GraphPrinterSettings.generated.h"

enum TextureFilter;
enum class EDesiredImageFormat : uint8;

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor)
class GRAPHPRINTER_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()
	
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

	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bDrawOnlyGraph;
	
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

	// Whether to overwrite the image if it already exists when export.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	bool bCanOverwriteFileWhenExport;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, Config, Category = "File")
	FDirectoryPath OutputDirectory;

public:
	// Constructor.
	UGraphPrinterSettings();
	
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static const UGraphPrinterSettings& Get();
	
protected:
	// UObject interface.
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.

	// Functions called when the value changes.
	void ModifyFormat();
	void ModifyCompressionQuality();
};
