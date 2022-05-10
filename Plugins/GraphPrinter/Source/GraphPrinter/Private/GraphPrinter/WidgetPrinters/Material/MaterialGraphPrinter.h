// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "MaterialGraphPrinter.generated.h"

/**
 * A class of graph printers dedicated to material assets.
 * Print the preview window along with the graph editor.
 */
UCLASS()
class UMaterialGraphPrinter : public UGenericGraphPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 MaterialGraphPrinterPrinter = GenericGraphPrinterPrinter + 50;

public:
	// UWidgetPrinter interface.
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const override;
	virtual int32 GetPriority() const override;
	// End of UWidgetPrinter interface.
};
