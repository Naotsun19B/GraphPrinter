// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "PrintDetailsPanelOptions.generated.h"

/**
 * An optional class to specify when printing the details panel.
 */
UCLASS()
class DETAILSPANELPRINTER_API UPrintDetailsPanelOptions : public UPrintWidgetOptions
{
	GENERATED_BODY()

public:
	// Constructor.
	UPrintDetailsPanelOptions();

	// UPrintWidgetOptions interface.
	virtual UPrintWidgetOptions* Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const override;
	// End of UPrintWidgetOptions interface.

public:
	// Expand all hierarchies when printing the details panel.
	bool bExpandHierarchyWhenPrinting;
};
