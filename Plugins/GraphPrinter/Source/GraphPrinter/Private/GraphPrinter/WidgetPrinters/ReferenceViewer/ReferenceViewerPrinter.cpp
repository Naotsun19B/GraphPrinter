// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/ReferenceViewer/ReferenceViewerPrinter.h"
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

#ifdef WITH_TEXT_CHUNK_HELPER
bool UReferenceViewerPrinter::CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const
{
	return false;
}
#endif

int32 UReferenceViewerPrinter::GetPriority() const
{
	return ReferenceViewerPrinterPrinter;
}

FString UReferenceViewerPrinter::GetGraphTitle(const TSharedPtr<SGraphEditorImpl>& GraphEditor) const
{
	if (const auto* ReferenceViewerGraph = Cast<UEdGraph_ReferenceViewer>(GraphEditor->GetCurrentGraph()))
	{
		const TArray<FAssetIdentifier>& Assets = ReferenceViewerGraph->GetCurrentGraphRootIdentifiers();
		if (Assets.IsValidIndex(0))
		{
			return FString::Printf(TEXT("ReferenceViewer_%s"), *FPaths::GetBaseFilename(Assets[0].PackageName.ToString()));
		}
	}

	return TEXT("InvalidReferenceViewer");
} 
