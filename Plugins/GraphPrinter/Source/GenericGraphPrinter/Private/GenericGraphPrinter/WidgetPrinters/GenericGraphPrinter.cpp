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
	auto* PrintOptions = NewObject<UPrintGraphOptions>();
	return PrintOptions;
}

TSharedRef<GraphPrinter::IInnerPrinter> UGenericGraphPrinter::CreateInnerPrinter() const
{
	return MakeShared<GraphPrinter::FGenericGraphPrinter>(
		GetPrintOptions<UPrintGraphOptions>(),
		GetRestoreOptions()
	);
}
