// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "PreviewViewportPrinter.generated.h"

/**
 * A class of graph printers dedicated to preview viewport in materials, animation blueprints, etc.
 */
UCLASS()
class PREVIEWVIEWPORTPRINTER_API UPreviewViewportPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 PreviewViewportPrinterPriority = 200;
	
public:
	// UWidgetPrinter interface.
	virtual int32 GetPriority() const override;
	virtual TSharedRef<GraphPrinter::IInnerPrinter> CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	virtual TSharedRef<GraphPrinter::IInnerPrinter> CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	// End of UWidgetPrinter interface.

	// Returns the drawing result of the preview viewport.
	static UTextureRenderTarget2D* GetRenderedPreviewViewport(UPrintWidgetOptions* Options);
};
