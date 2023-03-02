// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "ReferenceViewerPrinter/WidgetPrinters/InnerReferenceViewerPrinter.h"
#include "SGraphEditorImpl.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

namespace GraphPrinter
{
	FReferenceViewerPrinter::FReferenceViewerPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FReferenceViewerPrinter::FReferenceViewerPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	bool FReferenceViewerPrinter::CanPrintWidget() const
	{
		if (Super::CanPrintWidget())
		{
			const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
			return IsValid(Cast<UEdGraph_ReferenceViewer>(GraphEditor->GetCurrentGraph()));
		}

		return false;
	}

	FString FReferenceViewerPrinter::GetWidgetTitle()
	{
		if (const auto* ReferenceViewerGraph = Cast<UEdGraph_ReferenceViewer>(Widget->GetCurrentGraph()))
		{
			const TArray<FAssetIdentifier>& Assets = ReferenceViewerGraph->GetCurrentGraphRootIdentifiers();
			if (Assets.IsValidIndex(0))
			{
				return FString::Printf(TEXT("ReferenceViewer_%s"), *FPaths::GetBaseFilename(Assets[0].PackageName.ToString()));
			}
		}

		return TEXT("InvalidReferenceViewer");
	}
}
