// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "ReferenceViewerPrinter/WidgetPrinters/ReferenceViewerPrinter.h"
#include "GraphPrinter/Utilities/CastSlateWidget.h"
#include "SGraphEditorImpl.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

bool UReferenceViewerPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	if (GraphEditor.IsValid())
	{
		return IsValid(Cast<UEdGraph_ReferenceViewer>(GraphEditor->GetCurrentGraph()));
	}

	return false;
}

int32 UReferenceViewerPrinter::GetPriority() const
{
	return ReferenceViewerPrinterPrinter;
}

FString UReferenceViewerPrinter::GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const
{
	const TSharedPtr<SGraphEditorImpl>& GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		if (const auto* ReferenceViewerGraph = Cast<UEdGraph_ReferenceViewer>(GraphEditor->GetCurrentGraph()))
		{
			const TArray<FAssetIdentifier>& Assets = ReferenceViewerGraph->GetCurrentGraphRootIdentifiers();
			if (Assets.IsValidIndex(0))
			{
				return FString::Printf(TEXT("ReferenceViewer_%s"), *FPaths::GetBaseFilename(Assets[0].PackageName.ToString()));
			}
		}
	}

	return TEXT("InvalidReferenceViewer");
}
