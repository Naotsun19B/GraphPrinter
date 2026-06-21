// Copyright 2020-2026 Naotsun. All Rights Reserved.

#include "ClipboardImageExtension/Mac/MacClipboardImageExtension.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if PLATFORM_MAC && UE_5_08_OR_LATER
// From UE 5.8, Cocoa/AppKit headers are no longer transitively included via CoreMinimal on Mac,
// so Objective-C symbols (NSString, NSImage, NSPasteboard, NSArray, nil) must be brought in explicitly.
// Also, the newly introduced FImage type otherwise gets suggested in place of NSImage.
#import <Cocoa/Cocoa.h>
#endif

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
