// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"
#include "SGraphEditorImpl.h"
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class REFERENCEVIEWERPRINTER_API FReferenceViewerPrinter
		: public TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>;

	public:
		// Constructor.
		FReferenceViewerPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FReferenceViewerPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}

		// TInnerWidgetPrinter interface.
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget())
			{
				const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
				return IsValid(Cast<UEdGraph_ReferenceViewer>(GraphEditor->GetCurrentGraph()));
			}

			return false;
		}
		virtual FString GetWidgetTitle() override
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
		// End of TInnerWidgetPrinter interface.
	};
}
