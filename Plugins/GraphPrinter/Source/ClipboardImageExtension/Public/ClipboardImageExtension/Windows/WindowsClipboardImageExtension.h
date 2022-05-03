// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClipboardImageExtension/GenericPlatform/GenericClipboardImageExtension.h"

namespace ClipboardImageExtension
{
	/**
	 * An extension class for working with clipboard images on Windows.
	 */
	class CLIPBOARDIMAGEEXTENSION_API FWindowsClipboardImageExtension : public FGenericClipboardImageExtension
	{
	public:
		// FGenericClipboardImageExtension interface.
		static bool ClipboardCopy(const FString& Filename);
		static EDesiredImageFormat GetCopyableImageFormat();
		// End of FGenericClipboardImageExtension interface.
	};

#if WINDOWS_USE_FEATURE_APPLICATIONMISC_CLASS
	using FClipboardImageExtension = FWindowsClipboardImageExtension;
#endif
}
