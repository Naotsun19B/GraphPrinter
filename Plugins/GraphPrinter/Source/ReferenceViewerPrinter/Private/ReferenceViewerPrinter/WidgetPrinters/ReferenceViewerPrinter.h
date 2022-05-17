// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "ReferenceViewerPrinter.generated.h"

/**
 * A class of graph printers dedicated to reference viewers.
 */
UCLASS()
class REFERENCEVIEWERPRINTER_API UReferenceViewerPrinter : public UGenericGraphPrinter
{
	GENERATED_BODY()
	
public:
	// The priority of this widget printer.
	static constexpr int32 ReferenceViewerPrinterPrinter = GenericGraphPrinterPrinter + 100;
	
public:
	// UWidgetPrinter interface.
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const override;
	virtual int32 GetPriority() const override;
	virtual FString GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const override;
	// End of UWidgetPrinter interface.
};
