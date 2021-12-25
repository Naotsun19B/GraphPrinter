// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"
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
		, bCopyToClipboard(false)
		, bIsIncludeNodeInfoInImageFile(true)
		, bUseGamma(true)
		, Padding(0.f)
		, MaxImageSize(FVector2D::ZeroVector)
		, FilteringMode(TextureFilter::TF_Default)
		, bDrawOnlyGraph(false)
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

	// Whether to copy to the clipboard.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bCopyToClipboard;

	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the only output image format is png.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bIsIncludeNodeInfoInImageFile;
	
	// Whether to output the drawing result affected by the gamma value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bUseGamma;

	// Margins when drawing the graph editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	float Padding;

	// Set the maximum size of the output image. There is no limit at (0.f, 0.f).
	// If set it too high, you may run out of video memory and crash.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	FVector2D MaxImageSize;

	// Texture filtering mode to use when outputting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	TEnumAsByte<TextureFilter> FilteringMode;

	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	bool bDrawOnlyGraph;
	
	// Options related to image output processing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	FImageWriteOptions ImageWriteOptions;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrintGraphOptions")
	FString OutputDirectoryPath;

	// Used to specify the window to be output (native only).
	TSharedPtr<SWindow> TargetWindowOverride;
};
