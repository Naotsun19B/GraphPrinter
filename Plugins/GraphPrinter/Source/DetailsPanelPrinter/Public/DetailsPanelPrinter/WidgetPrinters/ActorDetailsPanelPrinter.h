// Copyright 2020-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DetailsPanelPrinter/WidgetPrinters/DetailsPanelPrinter.h"
#include "ActorDetailsPanelPrinter.generated.h"

/**
 * A printer class for the actor details panel.
 */
UCLASS()
class DETAILSPANELPRINTER_API UActorDetailsPanelPrinter : public UDetailsPanelPrinter
{
	GENERATED_BODY()

public:
	// UWidgetPrinter interface.
	virtual int32 GetPriority() const override;
	virtual TOptional<GraphPrinter::FSupportedWidget> CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const override;
	// End of UWidgetPrinter interface.
};
