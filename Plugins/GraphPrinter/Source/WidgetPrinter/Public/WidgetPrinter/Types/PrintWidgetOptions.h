// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture.h"
#include "ImageWriteBlueprintLibrary.h"
#include "PrintWidgetOptions.generated.h"

class SWidget;

/**
 * An optional class to specify when printing the widget.
 */
UCLASS()
class WIDGETPRINTER_API UPrintWidgetOptions : public UObject
{
	GENERATED_BODY()
	
public:
	// The type of range to print.
	enum class EPrintScope : uint8
	{
		All,
		Selected,
	};

	// The type of output destination for the printed image.
	enum class EExportMethod : uint8
	{
		RenderTarget,
		ImageFile,
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		Clipboard,
#endif
	};

public:
	// Constructor.
	UPrintWidgetOptions();

	// Creates and returns a duplicate as the specified option class.
	virtual UPrintWidgetOptions* Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const;
	template<class TPrintOptions>
	TPrintOptions* Duplicate()
	{
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintWidgetOptions.");

		return Cast<TPrintOptions>(Duplicate(TPrintOptions::StaticClass()));
	}
	
public:
	// The type of node range to print.
	EPrintScope PrintScope;

	// The type of output destination for the printed image.
	EExportMethod ExportMethod;

#ifdef WITH_TEXT_CHUNK_HELPER
	// Whether to embed node configuration information in the image file.
	// When this option is enabled, the image formats that can be selected are limited.
	bool bIsIncludeNodeInfoInImageFile;
#endif
		
	// Whether to output the drawing result affected by the gamma value.
	bool bUseGamma;

	// Set the maximum size of the output image. There is no limit at (0.f, 0.f).
	// If set it too high, you may run out of video memory and crash.
	FVector2D MaxImageSize;

	// If the scale for drawing the graph is 0.5, it will be drawn at half the resolution.
	// Decrease the value if you want to draw a graph larger than MaxImageSize.
	// Decreasing the value may cause defects in the drawing result.
	float RenderingScale;
		
	// Texture filtering mode to use when outputting.
	TEnumAsByte<TextureFilter> FilteringMode;
	
	// Options related to image output processing.
	FImageWriteOptions ImageWriteOptions;

	// Directory path where the image file is output.
	FString OutputDirectoryPath;

	// Used when you want to specify the widget to print as an argument.
	TSharedPtr<SWidget> SearchTarget;
};
