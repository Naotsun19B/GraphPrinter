// Copyright 2020-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "DetailsPanelPrinter.generated.h"

/**
 * A printer class for the details panel.
 */
UCLASS()
class DETAILSPANELPRINTER_API UDetailsPanelPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 DetailsPanelPrinterPriority = 200;
	
public:
	// UWidgetPrinter interface.
	virtual int32 GetPriority() const override;
	virtual TOptional<GraphPrinter::FSupportedWidget> CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const override;
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions(
		const UPrintWidgetOptions::EPrintScope PrintScope,
		const UPrintWidgetOptions::EExportMethod ExportMethod
	) const override;
	virtual URestoreWidgetOptions* CreateDefaultRestoreOptions() const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	// End of UWidgetPrinter interface.
};
