// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

namespace ClipboardImageExtension
{
	/**
	 * A extension class for working with images on the clipboard.
	 */
	class CLIPBOARDIMAGEEXTENSION_API FGenericClipboardImageExtension
	{
	public:
		// Returns whether the function to copy images to the clipboard is available.
		static bool IsCopyImageToClipboardAvailable() { return false; }
		
		// Copies the image file with the specified path to the clipboard.
		static bool ClipboardCopy(const FString& Filename) { return false; }

		// Returns an image format that can be copied to the clipboard.
		static EDesiredImageFormat GetCopyableImageFormat() { return EDesiredImageFormat::PNG; }
	};
}
