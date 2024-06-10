// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImageWriteBlueprintLibrary.h"
#include "Templates/SubclassOf.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#if UE_5_02_OR_LATER
#include "Engine/TextureDefines.h"
#endif
#include "PrintWidgetOptions.generated.h"

class SWidget;
#if !UE_5_02_OR_LATER
enum TextureFilter;
#endif

/**
 * An optional class to specify when printing the widget.
 */
UCLASS()
class WIDGETPRINTER_API UPrintWidgetOptions : public UObject
{
	GENERATED_BODY()
	
public:
	// An enum class that defines type of range to print.
	enum class EPrintScope : uint8
	{
		All,
		Selected,
	};

	// An enum class that defines type of output destination for the printed image.
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
	// Whether to embed the widget information in the image file.
	// When this option is enabled, the image formats that can be selected are limited.
	bool bIsIncludeWidgetInfoInImageFile;
#endif
		
	// Whether to output the drawing result affected by the gamma value.
	bool bUseGamma;

	// The maximum size of the output image.
	FVector2D MaxImageSize;

	// The drawing scale of the output image.
	float RenderingScale;
		
	// The texture filtering mode to use when outputting.
	TEnumAsByte<TextureFilter> FilteringMode;
	
	// The options related to image output processing.
	FImageWriteOptions ImageWriteOptions;

	// The directory path where the image file is output.
	FString OutputDirectoryPath;

	// The widget to search for a graph editor to draw on.
	TSharedPtr<SWidget> SearchTarget;
};
