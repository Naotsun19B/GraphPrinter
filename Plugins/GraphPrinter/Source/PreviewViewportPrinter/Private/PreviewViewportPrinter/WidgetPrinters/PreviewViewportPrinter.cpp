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

TOptional<GraphPrinter::FSupportedWidget> UPreviewViewportPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SGlobalPlayWorldActions> PreviewViewport = GraphPrinter::FPreviewViewportPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!PreviewViewport.IsValid())
	{
		return {};
	}

	FString PreviewViewportTitle;
	if (!GraphPrinter::FPreviewViewportPrinter::GetPreviewViewportTitle(PreviewViewport, PreviewViewportTitle))
	{
		// return {};
	}
	
	return GraphPrinter::FSupportedWidget(PreviewViewport.ToSharedRef(), PreviewViewportTitle, GetPriority());
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
