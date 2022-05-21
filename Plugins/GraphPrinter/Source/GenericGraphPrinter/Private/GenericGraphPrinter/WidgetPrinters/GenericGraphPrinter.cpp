// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPrinter;
}

UPrintWidgetOptions* UGenericGraphPrinter::CreateDefaultPrintOptions() const
{
	if (UPrintWidgetOptions* PrintWidgetOptions = Super::CreateDefaultPrintOptions())
	{
		if (auto* PrintGraphOptions = PrintWidgetOptions->Duplicate<UPrintGraphOptions>())
		{
			// #TODO: Get value from editor preference.
			PrintGraphOptions->bDrawOnlyGraph = false;

			return PrintGraphOptions;
		}
	}

	return nullptr;
}

TSharedRef<GraphPrinter::IInnerPrinter> UGenericGraphPrinter::CreatePrintModeInnerPrinter( const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FGenericGraphPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerPrinter> UGenericGraphPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FGenericGraphPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
