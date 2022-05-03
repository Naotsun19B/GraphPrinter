// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

namespace ClipboardImageExtension
{
	/**
	 * Extension class for working with images on the clipboard.
	 */
	class CLIPBOARDIMAGEEXTENSION_API FGenericClipboardImageExtension
	{
	public:
		// Copies the image file with the specified path to the clipboard.
		static bool ClipboardCopy(const FString& Filename) { return false; }

		// Returns an image format that can be copied to the clipboard.
		static EDesiredImageFormat GetCopyableImageFormat() { return EDesiredImageFormat::PNG; }
	};
}
