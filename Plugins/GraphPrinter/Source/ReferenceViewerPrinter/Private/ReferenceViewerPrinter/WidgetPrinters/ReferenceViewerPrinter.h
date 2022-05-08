// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterCore/WidgetPrinters/GenericGraphPrinter.h"
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
	virtual int32 GetPriority() const override;
	// End of UWidgetPrinter interface.
	
	// UGenericGraphPrinter interface.
	virtual FString GetGraphTitle(const TSharedPtr<SGraphEditorImpl>& GraphEditor) const override;
	// End of UGenericGraphPrinter interface.
};
