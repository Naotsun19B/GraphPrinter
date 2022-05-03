// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "ClipboardImageExtension/Windows/WindowsClipboardImageExtension.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"

#if WINDOWS_USE_FEATURE_APPLICATIONMISC_CLASS
#include "Windows/WindowsApplication.h"
#endif

namespace ClipboardImageExtension
{
#if WINDOWS_USE_FEATURE_APPLICATIONMISC_CLASS
	namespace ClipboardImageExtensionInternal
	{
		HBITMAP LoadBitmapImage(const FString& Filename)
		{
			TArray<uint8> RawFileData;
			if (!FFileHelper::LoadFileToArray(RawFileData, *Filename))
			{
				return nullptr;
			}
			
			auto& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
			if (!ImageWrapper.IsValid())
			{
				return nullptr;
			}

			if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.GetAllocatedSize()))
			{
				return nullptr;
			}

#if BEFORE_UE_4_24
			const TArray<uint8>* BitmapBuffer;
			if (!ImageWrapper->GetRaw(ImageWrapper->GetFormat(), ImageWrapper->GetBitDepth(), BitmapBuffer))
			{
				return nullptr;
			}
			if (BitmapBuffer == nullptr)
			{
				return nullptr;
			}
#else
			TArray<uint8> BitmapBuffer;
			if (!ImageWrapper->GetRaw(ImageWrapper->GetFormat(), ImageWrapper->GetBitDepth(), BitmapBuffer))
			{
				return nullptr;
			}
#endif

			return CreateBitmap(
				ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), 1, 32,
#if BEFORE_UE_4_24
				BitmapBuffer->GetData()
#else
				BitmapBuffer.GetData()
#endif
			);
		}
#endif
	}
	
	bool FWindowsClipboardImageExtension::ClipboardCopy(const FString& Filename)
	{
#if WINDOWS_USE_FEATURE_APPLICATIONMISC_CLASS
		const HBITMAP Bitmap = ClipboardImageExtensionInternal::LoadBitmapImage(Filename);
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
#else
		return false;
#endif
	}

	EDesiredImageFormat FWindowsClipboardImageExtension::GetCopyableImageFormat()
	{
		return EDesiredImageFormat::BMP;
	}
}
