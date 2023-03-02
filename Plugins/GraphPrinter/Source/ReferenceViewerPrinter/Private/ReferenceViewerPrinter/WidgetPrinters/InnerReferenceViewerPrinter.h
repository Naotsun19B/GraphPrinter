// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"

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
		FReferenceViewerPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FReferenceViewerPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);

		// IInnerWidgetPrinter interface.
		virtual bool CanPrintWidget() const override;
		// End of IInnerWidgetPrinter interface.

		// TInnerWidgetPrinter interface.
		virtual FString GetWidgetTitle() override;
		// End of TInnerWidgetPrinter interface.
	};
}
