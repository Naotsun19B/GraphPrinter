// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "ClipboardImageExtension/Linux/LinuxClipboardImageExtension.h"

namespace ClipboardImageExtension
{
	bool FLinuxClipboardImageExtension::IsCopyImageToClipboardAvailable()
	{
		const FString Command = FString::Printf(TEXT("which xclip > /dev/null 2>&1"));
		FProcHandle Handle = FPlatformProcess::CreateProc(
			*Command,
			nullptr,
			true,
			false,
			false,
			nullptr,
			0,
			nullptr,
			nullptr
		);
		
		int32 ReturnCode;
		if (!FPlatformProcess::GetProcReturnCode(Handle, &ReturnCode))
		{
			return false;
		}
		
		return (ReturnCode == 0);
	}

	bool FLinuxClipboardImageExtension::ClipboardCopy(const FString& Filename)
	{
		const FString Command = FString::Printf(
			TEXT("xclip -selection clipboard -t image/png -i %s"),
			*Filename
		);
		FProcHandle Handle = FPlatformProcess::CreateProc(
			*Command,
			nullptr,
			true,
			false,
			false,
			nullptr,
			0,
			nullptr,
			nullptr
		);

		int32 ReturnCode;
		if (!FPlatformProcess::GetProcReturnCode(Handle, &ReturnCode))
		{
			return false;
		}
		
		return (ReturnCode == 0);
	}

	EDesiredImageFormat FLinuxClipboardImageExtension::GetCopyableImageFormat()
	{
		return EDesiredImageFormat::PNG;
	}
}
