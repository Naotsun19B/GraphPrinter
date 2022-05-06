// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterCore/WidgetPrinter/WidgetPrinter.h"
#include "GenericGraphPrinter.generated.h"

/**
 * A general-purpose printer class for the graph editor.
 */
UCLASS()
class GRAPHPRINTERCORE_API UGenericGraphPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// UWidgetPrinter interface.
	virtual void PrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) override;
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const override;
#ifdef WITH_TEXT_CHUNK_HELPER
	virtual void RestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) override;
	virtual bool CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const override;
#endif
	virtual int32 GetPriority() const override;
	// End of UWidgetPrinter interface.
};
