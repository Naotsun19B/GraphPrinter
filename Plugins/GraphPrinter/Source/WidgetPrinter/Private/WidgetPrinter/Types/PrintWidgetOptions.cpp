// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/PrintWidgetOptions.h"

UPrintWidgetOptions::UPrintWidgetOptions()
	: PrintScope(EPrintScope::All)
	, ExportMethod(EExportMethod::ImageFile)
#ifdef WITH_TEXT_CHUNK_HELPER
	, bIsIncludeNodeInfoInImageFile(true)
#endif
	, bUseGamma(true)
	, MaxImageSize(FVector2D::ZeroVector)
	, RenderingScale(1.f)
	, FilteringMode(TextureFilter::TF_Default)
	, SearchTarget(nullptr)
{
	ImageWriteOptions.bAsync = true;
	ImageWriteOptions.bOverwriteFile = false;
	ImageWriteOptions.CompressionQuality = 0;
	ImageWriteOptions.Format = EDesiredImageFormat::PNG;
}

UPrintWidgetOptions* UPrintWidgetOptions::Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const
{
	auto* Destination = NewObject<UPrintWidgetOptions>(GetTransientPackage(), DestinationClass);
	if (IsValid(Destination))
	{
		Destination->PrintScope = PrintScope;
		Destination->ExportMethod = ExportMethod;
		Destination->bIsIncludeNodeInfoInImageFile = bIsIncludeNodeInfoInImageFile;
		Destination->bUseGamma = bUseGamma;
		Destination->MaxImageSize = MaxImageSize;
		Destination->RenderingScale = RenderingScale;
		Destination->FilteringMode = FilteringMode;
		Destination->ImageWriteOptions = ImageWriteOptions;
		Destination->OutputDirectoryPath = OutputDirectoryPath;
		Destination->SearchTarget = SearchTarget;
	}

	return Destination;
}
