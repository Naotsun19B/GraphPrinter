// Copyright 2020-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "MaterialGraphPrinter.generated.h"

/**
 * A class of graph printers dedicated to material assets.
 * Print the preview window along with the graph editor.
 */
UCLASS()
class MATERIALGRAPHPRINTER_API UMaterialGraphPrinter : public UGenericGraphPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 MaterialGraphPrinterPriority = (GenericGraphPrinterPriority + 50);
	
public:
	// UWidgetPrinter interface.
	virtual int32 GetPriority() const override;
	virtual TOptional<GraphPrinter::FSupportedWidget> CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const override;
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions(
		const UPrintWidgetOptions::EPrintScope PrintScope,
		const UPrintWidgetOptions::EExportMethod ExportMethod
	) const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	// End of UWidgetPrinter interface.
};
