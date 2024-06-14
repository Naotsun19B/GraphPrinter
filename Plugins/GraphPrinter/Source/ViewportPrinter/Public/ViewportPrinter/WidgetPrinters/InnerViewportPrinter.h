// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "Widgets/SViewport.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print viewport.
	 */
	class VIEWPORTPRINTER_API FViewportPrinter
		: public TInnerWidgetPrinter<SViewport, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerWidgetPrinter<SViewport, UPrintWidgetOptions, URestoreWidgetOptions>;
	
	public:
		// Constructor.
		FViewportPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FViewportPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SViewport> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override;
		virtual void PreDrawWidget() override;
		virtual void PostDrawWidget() override;
		virtual FString GetWidgetTitle() override;
		// End of TInnerWidgetPrinter interface.

		// Finds the target widget from the search target.
		static TSharedPtr<SViewport> FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget);
		
		// Returns the title from the viewport.
		static bool GetViewportTitle(const TSharedPtr<SViewport>& Viewport, FString& Title);

	protected:
		// A group of parameters that must be retained for processing.
		struct FViewportPrinterParams
		{
			// The original visibility to temporarily hide the viewport menu widget.
			EVisibility PreviousVisibility;
		};
		FViewportPrinterParams ViewportPrinterParams;
	};
}
