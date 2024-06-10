// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#if UE_5_02_OR_LATER
#include "Engine/TextureDefines.h"
#else
#include "Engine/Texture.h"
#endif
#include "UObject/Package.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PrintWidgetOptions)
#endif

UPrintWidgetOptions::UPrintWidgetOptions()
	: PrintScope(EPrintScope::All)
	, ExportMethod(EExportMethod::ImageFile)
#ifdef WITH_TEXT_CHUNK_HELPER
	, bIsIncludeWidgetInfoInImageFile(true)
#endif
	, bUseGamma(true)
	, MaxImageSize(FVector2D::ZeroVector)
	, RenderingScale(1.f)
	, FilteringMode(TF_Default)
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
		Destination->bIsIncludeWidgetInfoInImageFile = bIsIncludeWidgetInfoInImageFile;
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
