// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinter/Types/PrintWidgetOptions.h"
#include "GraphPrinter/Types/RestoreWidgetOptions.h"
#include "GraphPrinter/Types/OneWayBool.h"
#include "WidgetPrinter.generated.h"

class SWidget;

/**
 * A Class that prints widgets such as the graph editor.
 */
UCLASS(Abstract)
class GRAPHPRINTER_API UWidgetPrinter : public UObject
{
	GENERATED_BODY()

public:
	// Draw and export the widget with arguments.
	virtual void PrintWidget(GraphPrinter::FPrintWidgetOptions Options) PURE_VIRTUAL(UWidgetPrinterBase::PrintWidget, ; );
	
	// Returns whether the target widget can be printed.
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const PURE_VIRTUAL(UWidgetPrinterBase::CanPrintWidget, return false; );

#ifdef WITH_TEXT_CHUNK_HELPER
	// Restore the state of the widget from the image file.
	virtual void RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options) PURE_VIRTUAL(UWidgetPrinterBase::RestoreWidget, ; );

	// Returns whether the target widget can be restored.
	virtual bool CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const PURE_VIRTUAL(UWidgetPrinterBase::CanRestoreWidget, return false; );
#endif
	
	// Returns the printer priority.
	// Check if the printer can be executed from the one with the highest priority.
	virtual int32 GetPriority() const PURE_VIRTUAL(UWidgetPrinterBase::GetPriority, return 0; );
	
protected:
	// Draw the widget on the render target.
	virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget(
		const TSharedPtr<SWidget>& Widget,
		const FVector2D& DrawSize,
		const GraphPrinter::FPrintWidgetOptions& Options
	);

protected:
	// Number of attempts to draw the widget on the render target.
	// The drawing result may be corrupted once.
	// Probably if draw twice, the drawing result will not be corrupted.
	static constexpr int32 DrawTimes = 2;

	// The number of times to re-output as a countermeasure against the whitish image
	// that is output for the first time after starting the engine.
	static constexpr int32 NumberOfReOutputWhenFirstTime = 2;
	static GraphPrinter::FOneWayBool IsFirstOutput;
};
