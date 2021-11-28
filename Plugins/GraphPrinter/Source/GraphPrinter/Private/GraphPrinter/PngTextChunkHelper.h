// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/GraphPrinterGlobals.h"

#if ENABLE_EMBED_NODE_INFO

THIRD_PARTY_INCLUDES_START
// make sure no other versions of libpng headers are picked up.
#if WITH_LIBPNG_1_6
#include "ThirdParty/libPNG/libPNG-1.6.37/png.h"
#include "ThirdParty/libPNG/libPNG-1.6.37/pngstruct.h"
#include "ThirdParty/libPNG/libPNG-1.6.37/pnginfo.h"
#else
#include "ThirdParty/libPNG/libPNG-1.5.2/png.h"
#endif
THIRD_PARTY_INCLUDES_END

namespace GraphPrinter
{
	/**
	 * Helper class for accessing text chunks in png files.
	 */
	class GRAPHPRINTER_API FPngTextChunkHelper
	{
	public:
		// Constructor.
		FPngTextChunkHelper();

		// Destructor.
		virtual ~FPngTextChunkHelper();

		// Factory function that creates an instance of this class.
		static TSharedPtr<FPngTextChunkHelper> CreatePngTextChunkHelper(const FString& InFilename);

		// Write to text chunk of png file.
		virtual bool Write(const TMap<FString, FString>& MapToWrite);

		// Read into text chunks in png files.
		virtual bool Read(TMap<FString, FString>& MapToRead);

	protected:
		// Copy loaded data to member variables.
		virtual bool Initialize(const FString& InFilename, const void* InCompressedData, int64 InCompressedSize);

		// Check if the format of the loaded image file is png.
		virtual bool IsPng() const;

		// Callbacks for the libpng.
		static void UserReadCompressed(png_structp PngPtr, png_bytep Data, png_size_t Length);
		static void UserWriteCompressed(png_structp PngPtr, png_bytep Data, png_size_t Length);
		static void UserFlushData(png_structp PngPtr);
		static void UserError(png_structp PngPtr, png_const_charp ErrorMessage);
		static void UserWarning(png_structp PngPtr, png_const_charp WarningMessage);
		static void* UserMalloc(png_structp PngPtr, png_size_t Size);
		static void UserFree(png_structp PngPtr, png_voidp StructPtr);
		// End of callbacks for the libpng.

	protected:
		// File path of the original file.
		FString Filename;

		// Arrays of compressed data.
		TArray<uint8> CompressedData;

		// The read offset into our array.
		int64 ReadOffset;
	};
}
#endif
