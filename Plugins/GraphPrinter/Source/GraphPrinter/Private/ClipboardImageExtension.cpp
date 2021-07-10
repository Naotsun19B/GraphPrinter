// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "ClipboardImageExtension.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Modules/ModuleManager.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#else
#include "GraphPrinterCore.h"
#endif

namespace ClipboardImageExtensionInternal
{
#if PLATFORM_WINDOWS
	HBITMAP LoadBitmapImage(const FString& Filename)
	{
		TArray<uint8> RawFileData;
		if (!FFileHelper::LoadFileToArray(RawFileData, *Filename))
		{
			return nullptr;
		}
		
		auto& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
		if (!ImageWrapper.IsValid())
		{
			return nullptr;
		}

		if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.GetAllocatedSize()))
		{
			return nullptr;
		}

		TArray<uint8> BitmapBuffer;
		if (!ImageWrapper->GetRaw(ImageWrapper->GetFormat(), ImageWrapper->GetBitDepth(), BitmapBuffer))
		{
			return nullptr;
		}

		return CreateBitmap(
			ImageWrapper->GetWidth(), ImageWrapper->GetHeight(),
			1, 32, BitmapBuffer.GetData()
		);
	}
	
	bool ClipboardCopy(const FString& Filename)
	{
		const HBITMAP Bitmap = LoadBitmapImage(Filename);
		if (Bitmap == nullptr)
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Failed to load the file to copy to the clipboard (%s)"), *Filename);
			return false;
		}
	
		if (!OpenClipboard(GetActiveWindow()))
		{
			return false;
		}

		verify(EmptyClipboard());

		if (SetClipboardData(CF_BITMAP, Bitmap) == nullptr)
		{
			UE_LOG(LogGraphPrinter, Fatal, TEXT("SetClipboardData failed with error code %i"), static_cast<uint32>(GetLastError()));
		}

		verify(CloseClipboard());

		return true;
	}

	EDesiredImageFormat GetCopyableImageFormat()
	{
		return EDesiredImageFormat::BMP;
	}
#else
	void NotifyNotSupported()
	{
		FGraphPrinterCore::ShowNotification(
			NSLOCTEXT("ClipboardImageExtension", "NotSupportedPlatform", "This feature is not supported on this platform."),
			FGraphPrinterCore::CS_Fail
		);
	}
	
	bool ClipboardCopy(const FString& Filename)
	{
		NotifyNotSupported();
		return false;
	}

	EDesiredImageFormat GetCopyableImageFormat()
	{
		NotifyNotSupported();
		return {};
	}
#endif
}

bool FClipboardImageExtension::ClipboardCopy(const FString& Filename)
{
	return ClipboardImageExtensionInternal::ClipboardCopy(Filename);
}

EDesiredImageFormat FClipboardImageExtension::GetCopyableImageFormat()
{
	return ClipboardImageExtensionInternal::GetCopyableImageFormat();
}
