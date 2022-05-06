// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/WidgetPrinter/GenericGraphPrinter.h"

void UGenericGraphPrinter::PrintWidget(const GraphPrinter::FPrintWidgetOptions& Options)
{
	
}

bool UGenericGraphPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	return true;
}

#ifdef WITH_TEXT_CHUNK_HELPER
void UGenericGraphPrinter::RestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options)
{
	
}

bool UGenericGraphPrinter::CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const
{
	return true;
}
#endif

int32 UGenericGraphPrinter::GetPriority() const
{
	return 0;
}
