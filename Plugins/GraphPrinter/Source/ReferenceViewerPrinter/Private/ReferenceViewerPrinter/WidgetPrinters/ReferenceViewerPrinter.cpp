// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "ReferenceViewerPrinter/WidgetPrinters/ReferenceViewerPrinter.h"
#include "ReferenceViewerPrinter/WidgetPrinters/InnerReferenceViewerPrinter.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ReferenceViewerPrinter)
#endif

int32 UReferenceViewerPrinter::GetPriority() const
{
	return ReferenceViewerPrinterPriority;
}

TOptional<GraphPrinter::FSupportedWidget> UReferenceViewerPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SGraphEditorImpl> ReferenceViewerGraphEditor = GraphPrinter::FReferenceViewerPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!ReferenceViewerGraphEditor.IsValid())
	{
		return {};
	}

	FString ReferenceViewerGraphTitle;
	if (!GraphPrinter::FReferenceViewerPrinter::GetReferenceViewerGraphTitle(ReferenceViewerGraphEditor, ReferenceViewerGraphTitle))
	{
		return {};
	}
	
	return GraphPrinter::FSupportedWidget(ReferenceViewerGraphEditor.ToSharedRef(), ReferenceViewerGraphTitle, GetPriority());
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
