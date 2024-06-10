// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "PreviewViewportPrinter/WidgetPrinters/InnerPreviewViewportPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PreviewViewportPrinter)
#endif

int32 UPreviewViewportPrinter::GetPriority() const
{
	return PreviewViewportPrinterPriority;
}

FString UPreviewViewportPrinter::GetSupportedWidgetTypeName() const
{
	return GraphPrinter::FPreviewViewportPrinter::GetSupportedWidgetTypeName();
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UPreviewViewportPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FPreviewViewportPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UPreviewViewportPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FPreviewViewportPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
