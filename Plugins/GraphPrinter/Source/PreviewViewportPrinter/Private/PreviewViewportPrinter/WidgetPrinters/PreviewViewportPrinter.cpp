// Copyright 2020-2023 Naotsun. All Rights Reserved.

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
