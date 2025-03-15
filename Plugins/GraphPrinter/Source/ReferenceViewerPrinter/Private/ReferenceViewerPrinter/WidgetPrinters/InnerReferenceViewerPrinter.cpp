// Copyright 2020-2025 Naotsun. All Rights Reserved.

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
		FString Title;
		GetReferenceViewerGraphTitle(Widget, Title);
		return Title;
	}

	bool FReferenceViewerPrinter::GetReferenceViewerGraphTitle(const TSharedPtr<SGraphEditorImpl>& ReferenceViewerGraphEditor, FString& Title)
	{
		Title = TEXT("InvalidReferenceViewer");

		const auto* ReferenceViewerGraph = Cast<UEdGraph_ReferenceViewer>(ReferenceViewerGraphEditor->GetCurrentGraph());
		if (!IsValid(ReferenceViewerGraph))
		{
			return false;
		}
		
		const TArray<FAssetIdentifier>& Assets = ReferenceViewerGraph->GetCurrentGraphRootIdentifiers();
#if UE_5_00_OR_LATER
		if (Assets.IsEmpty())
#else
		if (Assets.Num() == 0)
#endif
		{
			return false;
		}
		
		Title = FString::Printf(TEXT("ReferenceViewer-%s"), *FPaths::GetBaseFilename(Assets[0].PackageName.ToString()));
		return true;
	}
}
