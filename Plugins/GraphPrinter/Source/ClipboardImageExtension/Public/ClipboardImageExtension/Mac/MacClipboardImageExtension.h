// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClipboardImageExtension/GenericPlatform/GenericClipboardImageExtension.h"

namespace ClipboardImageExtension
{
	/**
	 * An extension class for working with clipboard images on Mac.
	 */
	class CLIPBOARDIMAGEEXTENSION_API FMacClipboardImageExtension : public FGenericClipboardImageExtension
	{
	public:
		// FGenericClipboardImageExtension interface.
		static bool IsCopyImageToClipboardAvailable();
		static bool ClipboardCopy(const FString& Filename);
		static EDesiredImageFormat GetCopyableImageFormat();
		// End of FGenericClipboardImageExtension interface.
	};

#if PLATFORM_MAC
	using FClipboardImageExtension = FMacClipboardImageExtension;
#endif
}
