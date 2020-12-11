// Copyright 2020 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteBlueprintLibrary.h"
#include "Engine/Texture.h"
#include "GraphPrinterTypes.generated.h"

class SWidget;
class SWindow;

/**
 * Option set for executing the PrintGraph command.
 */
USTRUCT(BlueprintType)
struct FPrintGraphOptions
{
	GENERATED_BODY()

public:
	FPrintGraphOptions()
		: bOnlySelectedNodes(false)
		, Padding(0.f)
		, bUseGamma(true)
		, FilteringMode(TextureFilter::TF_Default)
		, bIsIncludeNodeInfoInImageFile(true)
		, OutputDirectoryPath()
		, TargetWindowOverride(nullptr)
	{
		ImageWriteOptions.bAsync = true;
		ImageWriteOptions.bOverwriteFile = false;
		ImageWriteOptions.CompressionQuality = 0;
		ImageWriteOptions.Format = EDesiredImageFormat::PNG;
	}

	// Whether to target only the selected node.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bOnlySelectedNodes;

	// Margins when drawing the graph editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	float Padding;

	// Whether to output the drawing result affected by the gamma value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bUseGamma;

	// Texture filtering mode to use when outputting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	TEnumAsByte<TextureFilter> FilteringMode;

	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the only output image format is png.
	UPROPERTY(EditAnywhere, Config, Category = "Image")
	bool bIsIncludeNodeInfoInImageFile;

	// Options related to image output processing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	FImageWriteOptions ImageWriteOptions;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	FString OutputDirectoryPath;

	// Used to specify the window to be output (native only).
	TSharedPtr<SWindow> TargetWindowOverride;
};

/**
 * Types of things that users of editor notifications can interact with.
 */
UENUM()
enum class ENotificationInteraction : uint8 
{
	None,
	Hyperlink,
	Button,
};
