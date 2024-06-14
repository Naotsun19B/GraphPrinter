// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "ViewportPrinter/WidgetPrinters/ViewportPrinter.h"	
#include "ViewportPrinter/WidgetPrinters/InnerViewportPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ViewportPrinter)
#endif

int32 UViewportPrinter::GetPriority() const
{
	return ViewportPrinterPriority;
}

TOptional<GraphPrinter::FSupportedWidget> UViewportPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SViewport> Viewport = GraphPrinter::FViewportPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!Viewport.IsValid())
	{
		return {};
	}

	FString ViewportTitle;
	if (!GraphPrinter::FViewportPrinter::GetViewportTitle(Viewport, ViewportTitle))
	{
		return {};
	}
	
	return GraphPrinter::FSupportedWidget(Viewport.ToSharedRef(), ViewportTitle, GetPriority());
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UViewportPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FViewportPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UViewportPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FViewportPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
