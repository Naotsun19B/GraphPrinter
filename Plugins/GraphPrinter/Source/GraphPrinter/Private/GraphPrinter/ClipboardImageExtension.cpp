// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinter/ClipboardImageExtension.h"
#include "GraphPrinter/GraphPrinterGlobals.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#endif

namespace GraphPrinter
{
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
			UE_LOG(LogGraphPrinter, Error, TEXT("This feature is not supported on this platform."));
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
}
