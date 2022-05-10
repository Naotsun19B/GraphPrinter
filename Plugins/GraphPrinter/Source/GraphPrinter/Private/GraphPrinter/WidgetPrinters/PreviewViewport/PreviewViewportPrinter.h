// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/WidgetPrinters/WidgetPrinter.h"
#include "PreviewViewportPrinter.generated.h"

/**
 * A class of graph printers dedicated to preview viewport in materials, animation blueprints, etc.
 */
UCLASS()
class UPreviewViewportPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 PreviewViewportPrinterPrinter = 200;

public:
	// Specify the widget to be searched and get the drawing result of the preview viewport.
	static UTextureRenderTarget2D* GetRenderedPreviewViewport(
		const TSharedPtr<SWidget>& TargetWidget,
		const GraphPrinter::FPrintWidgetOptions& Options
	);
	
	// UWidgetPrinter interface.
	virtual void PrintWidget(GraphPrinter::FPrintWidgetOptions Options) override;
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const override;
#ifdef WITH_TEXT_CHUNK_HELPER
	virtual void RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options) override;
	virtual bool CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const override;
#endif
	virtual int32 GetPriority() const override;
	virtual FString GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const override;
	// End of UWidgetPrinter interface.

protected:
	// The version of GetRenderedPreviewViewport used internally.
	static UTextureRenderTarget2D* GetRenderedPreviewViewportInternal(
		TSharedPtr<SWidget>& PreviewViewport,
		const GraphPrinter::FPrintWidgetOptions& Options
	);
	
	// Returns the preview viewport to print.
	virtual TSharedPtr<SWidget> FindPreviewViewport(const TSharedPtr<SWidget>& TargetWidget) const;

	// Performs processing before shooting the preview viewport.
	virtual void PrePrintPreviewViewport(
		EVisibility& PreviousVisibility,
		const TSharedPtr<SWidget>& PreviewViewport, 
		const GraphPrinter::FPrintWidgetOptions& Options
	);

	// Performs processing after shooting the preview viewport.
	virtual void PostPrintPreviewViewport(
		const TSharedPtr<SWidget>& PreviewViewport,
		const EVisibility& PreviousVisibility,
		const GraphPrinter::FPrintWidgetOptions& Options
	);
};
