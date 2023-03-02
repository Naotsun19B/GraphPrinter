// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "Kismet2/DebuggerCommands.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class PREVIEWVIEWPORTPRINTER_API FPreviewViewportPrinter
		: public TInnerWidgetPrinter<SGlobalPlayWorldActions, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerWidgetPrinter<SGlobalPlayWorldActions, UPrintWidgetOptions, URestoreWidgetOptions>;
	
	public:
		// Constructor.
		FPreviewViewportPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FPreviewViewportPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SGlobalPlayWorldActions> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override;
		virtual void PreDrawWidget() override;
		virtual void PostDrawWidget() override;
		virtual FString GetWidgetTitle() override;
		// End of TInnerWidgetPrinter interface.

	protected:
		// A group of parameters that must be retained for processing.
		struct FPreviewViewportPrinterParams
		{
			// Keep the original visibility to temporarily hide the viewport menu widget.
			EVisibility PreviousVisibility;
		};
		FPreviewViewportPrinterParams PreviewViewportPrinterParams;
	};
}
