// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "RestoreDetailsPanelOptions.generated.h"

/**
 *  An optional class to specify when restore the state of the details panel.
 */
UCLASS()
class DETAILSPANELPRINTER_API URestoreDetailsPanelOptions : public URestoreWidgetOptions
{
	GENERATED_BODY()

public:
	// Constructor.
	URestoreDetailsPanelOptions();

	// URestoreWidgetOptions interface.
	virtual URestoreWidgetOptions* Duplicate(const TSubclassOf<URestoreWidgetOptions>& DestinationClass) const override;
	// End of URestoreWidgetOptions interface.

public:
	// Whether to also restore the expanded state when restoring.
	bool bWhetherToAlsoRestoreExpandedStates;
};
