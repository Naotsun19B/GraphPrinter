// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "PreviewViewportPrinter/WidgetPrinters/InnerPreviewViewportPrinter.h"

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
