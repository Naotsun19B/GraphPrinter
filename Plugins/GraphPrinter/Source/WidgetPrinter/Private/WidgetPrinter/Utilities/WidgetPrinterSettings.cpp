// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "ImageWriteTypes.h"
#include "Engine/Texture.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#define LOCTEXT_NAMESPACE "WidgetPrinterSettings"

UWidgetPrinterSettings::UWidgetPrinterSettings()
	: Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bUseGamma(true)
	, Padding(100.f)
	, MaxImageSize(15000.f, 15000.f)
	, RenderingScale(1.f)
	, bCanOverwriteFileWhenExport(false)
{
#ifdef WITH_TEXT_CHUNK_HELPER
	bIsIncludeNodeInfoInImageFile = true;
	bWithTextChunkHelper = true;
#else
	bIsIncludeNodeInfoInImageFile = false;
	bWithTextChunkHelper = false;
#endif
	
	OutputDirectory.Path = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectSavedDir(),
			GraphPrinter::PluginName.ToString()
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
	bIsIncludeNodeInfoInImageFile = false;
#endif

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

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UWidgetPrinterSettings, bIsIncludeNodeInfoInImageFile))
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

FText UWidgetPrinterSettings::GetDetailsTitle() const
{
	return LOCTEXT("DetailsTitle", "Widget Printer");
}

void UWidgetPrinterSettings::ModifyFormat()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	if (bIsIncludeNodeInfoInImageFile)
	{
		Format = EDesiredImageFormat::PNG;
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
