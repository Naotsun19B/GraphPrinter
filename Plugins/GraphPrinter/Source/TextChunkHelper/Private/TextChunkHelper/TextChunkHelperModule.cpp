// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "IImageWrapper.h"
#include "TextChunkHelper/ITextChunkHelper.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Misc/FileHelper.h"
#include "IImageWrapperModule.h"

#if WITH_UNREALPNG
#include "TextChunkHelper/Png/PngTextChunk.h"
#endif

namespace TextChunkHelper
{
	const FName ITextChunkHelper::PluginModuleName = TEXT("TextChunkHelper");
	
	class FTextChunkHelperModule : public ITextChunkHelper
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

		// ITextChunkHelper interface.
		virtual TSharedPtr<ITextChunk> CreateTextChunk(const FString& InFilename) const override;
		virtual bool IsSupportedImageFormat(const EDesiredImageFormat ImageFormat) const override;
		virtual void RegisterTextChunkGenerator(
			const EDesiredImageFormat ImageFormat,
			const FOnCreateTextChunk& GenerationProcess
		) override;
		virtual void UnregisterTextChunkGenerator(const EDesiredImageFormat ImageFormat) override;
		// End of ITextChunkHelper interface.

	private:
		// Text chunk generation process for each image format.
		TMap<EDesiredImageFormat, FOnCreateTextChunk> GenerationProcesses;
	};
	
	void FTextChunkHelperModule::StartupModule()
	{
#if WITH_UNREALPNG
		RegisterTextChunkGenerator(
			EDesiredImageFormat::PNG,
			FOnCreateTextChunk::CreateLambda(
				[]() -> TSharedPtr<ITextChunk>
				{
					return MakeShared<FPngTextChunk>();
				}
			)
		);
#endif
	}

	void FTextChunkHelperModule::ShutdownModule()
	{
#if WITH_UNREALPNG
		UnregisterTextChunkGenerator(EDesiredImageFormat::PNG);
#endif
	}

	TSharedPtr<ITextChunk> FTextChunkHelperModule::CreateTextChunk(const FString& InFilename) const
	{
		TArray<uint8> CompressedData;
		if (!FFileHelper::LoadFileToArray(CompressedData, *InFilename))
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Failed to load the file : %s"), *InFilename);
			return nullptr;
		}

		auto ToDesiredImageFormat = [](const EImageFormat ImageFormat) -> TOptional<EDesiredImageFormat>
		{
			switch (ImageFormat)
			{
			case EImageFormat::PNG:
				return EDesiredImageFormat::PNG;
			case EImageFormat::JPEG:
				return EDesiredImageFormat::JPG;
			case EImageFormat::BMP:
				return EDesiredImageFormat::BMP;
			case EImageFormat::EXR:
				return EDesiredImageFormat::EXR;
			default:
				break;
			};

			return {};
		};

		auto& ImageWrapper = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
		const TOptional<EDesiredImageFormat> ImageFormat = ToDesiredImageFormat(ImageWrapper.DetectImageFormat(
			CompressedData.GetData(),
			CompressedData.Num()
		));

		if (!ImageFormat.IsSet())
		{
			return nullptr;
		}
		
		const FOnCreateTextChunk* GenerationProcess = GenerationProcesses.Find(ImageFormat.GetValue());
		if (GenerationProcess == nullptr)
		{
			return nullptr;
		}

		check(GenerationProcess->IsBound());

		TSharedPtr<ITextChunk> TextChunk = GenerationProcess->Execute();
		if (!TextChunk.IsValid())
		{
			return nullptr;
		}

		ITextChunk::FInitializer Initializer(TextChunk.Get());
		if (!Initializer.Initialize(InFilename, CompressedData.GetData(), CompressedData.Num()))
		{
			return nullptr;
		}
		
		return TextChunk;
	}

	bool FTextChunkHelperModule::IsSupportedImageFormat(const EDesiredImageFormat ImageFormat) const
	{
		return GenerationProcesses.Contains(ImageFormat);
	}

	void FTextChunkHelperModule::RegisterTextChunkGenerator(
		const EDesiredImageFormat ImageFormat,
		const FOnCreateTextChunk& GenerationProcess
	)
	{
		if (FOnCreateTextChunk* ExistingProcess = GenerationProcesses.Find(ImageFormat))
		{
			*ExistingProcess = GenerationProcess;
			UE_LOG(LogGraphPrinter, Log, TEXT("Overwrote the processing of %s."), *UEnum::GetDisplayValueAsText(ImageFormat).ToString());
		}
		else
		{
			GenerationProcesses.Add(ImageFormat, GenerationProcess);
		}
	}

	void FTextChunkHelperModule::UnregisterTextChunkGenerator(const EDesiredImageFormat ImageFormat)
	{
		GenerationProcesses.Remove(ImageFormat);
	}
}

IMPLEMENT_MODULE(TextChunkHelper::FTextChunkHelperModule, TextChunkHelper)
