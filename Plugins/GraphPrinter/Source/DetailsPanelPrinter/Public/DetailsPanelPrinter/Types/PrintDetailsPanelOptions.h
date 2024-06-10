// Copyright 2020-2024 Naotsun. All Rights Reserved.

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
	// The height margin when drawing the details view.
	float Padding;
	
	// Whether to embed the expanded state of each item in the image file and restore the expanded state of each item when restoring.
	bool bIsIncludeExpansionStateInImageFile;
};
