// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "ReferenceViewerPrinter/WidgetPrinters/ReferenceViewerPrinter.h"
#include "ReferenceViewerPrinter/WidgetPrinters/InnerReferenceViewerPrinter.h"

int32 UReferenceViewerPrinter::GetPriority() const
{
	return ReferenceViewerPrinterPriority;
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UReferenceViewerPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FReferenceViewerPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UReferenceViewerPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FReferenceViewerPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
