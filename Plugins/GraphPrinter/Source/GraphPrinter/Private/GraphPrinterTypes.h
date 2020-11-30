// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteBlueprintLibrary.h"
#include "GraphPrinterTypes.generated.h"

class SWidget;
enum TextureFilter;

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
		, FilteringMode(TF_Default)
		, ImageWriteOptions()
		, OutputDirectoryPath()
		, TargetWindowOverride(nullptr)
	{
	}

	// Whether to target only the selected node.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlySelectedNodes;

	// Margins when drawing the graph editor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Padding;

	// Whether to output the drawing result affected by the gamma value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseGamma;

	// Texture filtering mode to use when outputting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TextureFilter> FilteringMode;

	// Options related to image output processing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FImageWriteOptions ImageWriteOptions;

	// Directory path where the image file is output.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
