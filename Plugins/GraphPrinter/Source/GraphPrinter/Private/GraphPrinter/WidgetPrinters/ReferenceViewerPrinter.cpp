// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/ReferenceViewerPrinter.h"
#include "SGraphEditorImpl.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

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
