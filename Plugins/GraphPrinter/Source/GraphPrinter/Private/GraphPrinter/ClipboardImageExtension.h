// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

namespace GraphPrinter
{
	/**
	 * Extension class for working with images on the clipboard.
	 */
	class GRAPHPRINTER_API FClipboardImageExtension
	{
	public:
		// Copies the image file with the specified path to the clipboard.
		static bool ClipboardCopy(const FString& Filename);

		// Returns an image format that can be copied to the clipboard.
		static EDesiredImageFormat GetCopyableImageFormat();
	};
}
