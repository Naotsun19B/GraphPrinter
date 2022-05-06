// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteBlueprintLibrary.h"

namespace GraphPrinter
{
	// The type of range to print.
	enum class EPrintScope : uint8
	{
		All,
		Selected,
	};

	// The type of output destination for the printed image.
	enum class EExportMethod : uint8
	{
		ImageFile,
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		Clipboard,
#endif
	};
	
	/**
	 * An optional structure to specify when printing the widget.
	 */
	struct FPrintWidgetOptions
	{
	public:
		// The type of node range to print.
		EPrintScope PrintScope;

		// The type of output destination for the printed image.
		EExportMethod ExportMethod;

#ifdef WITH_TEXT_CHUNK_HELPER
		// Whether to embed node configuration information in the image file.
		// When this option is enabled, the only output image format is png.
		bool bIsIncludeNodeInfoInImageFile;
#endif
		
		// Whether to output the drawing result affected by the gamma value.
		bool bUseGamma;

		// Margins when drawing the graph editor.
		float Padding;

		// Set the maximum size of the output image. There is no limit at (0.f, 0.f).
		// If set it too high, you may run out of video memory and crash.
		FVector2D MaxImageSize;

		// Texture filtering mode to use when outputting.
		TEnumAsByte<TextureFilter> FilteringMode;

		// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
		bool bDrawOnlyGraph;
	
		// Options related to image output processing.
		FImageWriteOptions ImageWriteOptions;

		// Directory path where the image file is output.
		FString OutputDirectoryPath;

		// Used when you want to specify the widget to print as an argument.
		TSharedPtr<SWidget> TargetWidget;

	public:
		// Constructor.
		FPrintWidgetOptions()
			: PrintScope(EPrintScope::All)
			, ExportMethod(EExportMethod::ImageFile)
#ifdef WITH_TEXT_CHUNK_HELPER
			, bIsIncludeNodeInfoInImageFile(true)
#endif
			, bUseGamma(true)
			, Padding(0.f)
			, MaxImageSize(FVector2D::ZeroVector)
			, FilteringMode(TextureFilter::TF_Default)
			, bDrawOnlyGraph(false)
			, TargetWidget(nullptr)
		{
			ImageWriteOptions.bAsync = true;
			ImageWriteOptions.bOverwriteFile = false;
			ImageWriteOptions.CompressionQuality = 0;
			ImageWriteOptions.Format = EDesiredImageFormat::PNG;
		}
	};
}
