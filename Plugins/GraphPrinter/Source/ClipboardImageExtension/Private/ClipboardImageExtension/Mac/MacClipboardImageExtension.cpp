// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "ClipboardImageExtension/Mac/MacClipboardImageExtension.h"

namespace ClipboardImageExtension
{
	bool FMacClipboardImageExtension::IsCopyImageToClipboardAvailable()
	{
		return true;
	}

	bool FMacClipboardImageExtension::ClipboardCopy(const FString& Filename)
	{
		bool bWasCopySuccessful = false;
		
#if PLATFORM_MAC
		NSString* Path = [NSString stringWithUTF8String:TCHAR_TO_UTF8(*Filename)];
		NSImage* Image = [[NSImage alloc] initWithContentsOfFile:Path];

		bWasCopySuccessful = (Image != nil);
		if (bWasCopySuccessful)
		{
			NSPasteboard* Pasteboard = [NSPasteboard generalPasteboard];
			[Pasteboard clearContents];
			[Pasteboard writeObjects:@[Image]];
		}

		[Image release];
#endif
		
		return bWasCopySuccessful;
	}

	EDesiredImageFormat FMacClipboardImageExtension::GetCopyableImageFormat()
	{
		return EDesiredImageFormat::JPG;
	}
}
