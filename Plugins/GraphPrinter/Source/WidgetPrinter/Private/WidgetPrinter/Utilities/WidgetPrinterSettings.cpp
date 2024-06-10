// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif
#include "ImageWriteTypes.h"
#include "Engine/Texture.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(WidgetPrinterSettings)
#endif

#define LOCTEXT_NAMESPACE "WidgetPrinterSettings"

UWidgetPrinterSettings::UWidgetPrinterSettings()
	: Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bUseGamma(true)
	, MaxImageSize(15000.f, 15000.f)
	, RenderingScale(1.f)
	, bCanOverwriteFileWhenExport(false)
{
#ifdef WITH_TEXT_CHUNK_HELPER
	bIsIncludeWidgetInfoInImageFile = true;
#else
	bIsIncludeWidgetInfoInImageFile = false;
#endif
	
	OutputDirectory.Path = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectSavedDir(),
			GraphPrinter::Global::PluginName.ToString()
		)
	);
}

const UWidgetPrinterSettings& UWidgetPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UWidgetPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UWidgetPrinterSettings::PostInitProperties()
{
	Super::PostInitProperties();

#ifndef WITH_TEXT_CHUNK_HELPER
	bIsIncludeWidgetInfoInImageFile = false;
#endif

	// If you don't have a directory, you can't open it with Explorer, so create it first.
	auto& FileManager = IFileManager::Get();
	if (!FileManager.DirectoryExists(*OutputDirectory.Path))
	{
		FileManager.MakeDirectory(*OutputDirectory.Path, true);
	}

	ModifyFormat();
	ModifyCompressionQuality();
	ModifyMaxImageSize();
}

void UWidgetPrinterSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, bIsIncludeWidgetInfoInImageFile) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, Format))
	{
		ModifyFormat();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, Format) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, CompressionQuality))
	{
		ModifyCompressionQuality();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, MaxImageSize))
	{
		ModifyMaxImageSize();
	}
}

#if UE_4_25_OR_LATER
bool UWidgetPrinterSettings::CanEditChange(const FProperty* InProperty) const
#else
bool UWidgetPrinterSettings::CanEditChange(const UProperty* InProperty) const
#endif
{
	if (InProperty != nullptr)
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, bIsIncludeWidgetInfoInImageFile))
		{
#ifndef WITH_TEXT_CHUNK_HELPER
			return false;
#endif
		}
	}
	
	return Super::CanEditChange(InProperty);
}

UGraphPrinterSettings::FSettingsInfo UWidgetPrinterSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("WidgetPrinter"));
}

void UWidgetPrinterSettings::ModifyFormat()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	if (bIsIncludeWidgetInfoInImageFile)
	{
		if (!TextChunkHelper::ITextChunkHelper::Get().IsSupportedImageFormat(Format))
		{
			const FText Message = FText::Format(
				LOCTEXT("FormatWarningText", "The specified image format ({Extension}) does not support text chunks."),
				FText::FromString(GraphPrinter::FGraphPrinterUtils::GetImageFileExtension(Format, false))
			);
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				Message,
				GraphPrinter::FGraphPrinterUtils::CS_Fail
			);
			
			bool bFoundSupportedImageFormat = false;
			if (const UEnum* DesiredImageFormatEnum = StaticEnum<EDesiredImageFormat>())
			{
				for (int32 Index = 0; Index < DesiredImageFormatEnum->NumEnums() -1; Index++)
				{
					const auto DesiredImageFormat = static_cast<EDesiredImageFormat>(Index);
					if (TextChunkHelper::ITextChunkHelper::Get().IsSupportedImageFormat(DesiredImageFormat))
					{
						Format = DesiredImageFormat;
						bFoundSupportedImageFormat = true;
					}
				}
			}
			if (!bFoundSupportedImageFormat)
			{
				bIsIncludeWidgetInfoInImageFile = false;
			}
		}
	}
#endif
}

void UWidgetPrinterSettings::ModifyCompressionQuality()
{
	int32 Max = 100;
	if (Format == EDesiredImageFormat::EXR)
	{
		Max = 1;
	}
	CompressionQuality = FMath::Clamp(CompressionQuality, 0, Max);
}

void UWidgetPrinterSettings::ModifyMaxImageSize()
{
	if (MaxImageSize.X < 0.f)
	{
		MaxImageSize.X = 0.f;
	}
	if (MaxImageSize.Y < 0.f)
	{
		MaxImageSize.Y = 0.f;
	}
}

#undef LOCTEXT_NAMESPACE
