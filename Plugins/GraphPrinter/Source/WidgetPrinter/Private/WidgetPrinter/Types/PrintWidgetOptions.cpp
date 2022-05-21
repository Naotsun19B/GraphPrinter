// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/PrintWidgetOptions.h"

UPrintWidgetOptions::UPrintWidgetOptions()
	: PrintScope(EPrintScope::All)
	, ExportMethod(EExportMethod::ImageFile)
#ifdef WITH_TEXT_CHUNK_HELPER
	, bIsIncludeNodeInfoInImageFile(true)
#endif
	, bUseGamma(true)
	, Padding(0.f)
	, MaxImageSize(FVector2D::ZeroVector)
	, RenderingScale(0.f)
	, FilteringMode(TextureFilter::TF_Default)
	, TargetWidget(nullptr)
{
	ImageWriteOptions.bAsync = true;
	ImageWriteOptions.bOverwriteFile = false;
	ImageWriteOptions.CompressionQuality = 0;
	ImageWriteOptions.Format = EDesiredImageFormat::PNG;
}
