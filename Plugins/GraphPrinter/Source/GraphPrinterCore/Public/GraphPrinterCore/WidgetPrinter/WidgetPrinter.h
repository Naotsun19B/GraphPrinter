// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterCore/Types/PrintWidgetOptions.h"
#include "GraphPrinterCore/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter.generated.h"

/**
 * A Class that prints widgets such as the graph editor.
 */
UCLASS(Abstract)
class GRAPHPRINTERCORE_API UWidgetPrinter : public UObject
{
	GENERATED_BODY()

public:
	// Draw and export the widget with arguments.
	virtual void PrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) PURE_VIRTUAL(UWidgetPrinterBase::PrintWidget, ; );
	
	// Returns whether the target widget can be printed.
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const PURE_VIRTUAL(UWidgetPrinterBase::CanPrintWidget, return false; );

#ifdef WITH_TEXT_CHUNK_HELPER
	// Restore the state of the widget from the image file.
	virtual void RestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) PURE_VIRTUAL(UWidgetPrinterBase::RestoreWidget, ; );

	// Returns whether the target widget can be restored.
	virtual bool CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const PURE_VIRTUAL(UWidgetPrinterBase::CanRestoreWidget, return false; );
#endif
	
	// Returns the printer priority.
	// Check if the printer can be executed from the one with the highest priority.
	virtual int32 GetPriority() const PURE_VIRTUAL(UWidgetPrinterBase::GetPriority, return 0; );
};
