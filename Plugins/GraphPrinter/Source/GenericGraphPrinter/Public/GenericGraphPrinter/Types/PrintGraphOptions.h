// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "PrintGraphOptions.generated.h"

/**
 * An optional class to specify when printing the graph editor.
 */
UCLASS()
class GENERICGRAPHPRINTER_API UPrintGraphOptions : public UPrintWidgetOptions
{
	GENERATED_BODY()

public:
	// Constructor.
	UPrintGraphOptions();

	// UPrintWidgetOptions interface.
	virtual UPrintWidgetOptions* Duplicate(const TSubclassOf<UPrintWidgetOptions>& DestinationClass) const override;
	// End of UPrintWidgetOptions interface.

public:
	// Margins when drawing the graph editor.
	float Padding;
	
	// Whether to hide the title bar of the graph editor and the text of the graph type in the lower right.
	bool bDrawOnlyGraph;
};
