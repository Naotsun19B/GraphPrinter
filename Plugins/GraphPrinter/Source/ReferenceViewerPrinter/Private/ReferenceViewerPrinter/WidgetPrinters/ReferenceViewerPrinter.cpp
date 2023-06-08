// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "ReferenceViewerPrinter/WidgetPrinters/ReferenceViewerPrinter.h"
#include "ReferenceViewerPrinter/WidgetPrinters/InnerReferenceViewerPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ReferenceViewerPrinter)
#endif

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
