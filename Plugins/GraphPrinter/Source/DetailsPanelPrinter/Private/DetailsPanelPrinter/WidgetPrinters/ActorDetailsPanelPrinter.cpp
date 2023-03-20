// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/ActorDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"

UActorDetailsPanelPrinter::UActorDetailsPanelPrinter()
{
#ifndef WITH_DETAILS_PANEL_PRINTER
	if (UClass* Class = GetClass())
	{
		Class->ClassFlags |= CLASS_Abstract;
	}
#endif
}

int32 UActorDetailsPanelPrinter::GetPriority() const
{
	return DetailsPanelPrinterPriority + 1;
}

#ifdef WITH_DETAILS_PANEL_PRINTER
TSharedRef<GraphPrinter::IInnerWidgetPrinter> UActorDetailsPanelPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FActorDetailsPanelPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UActorDetailsPanelPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FActorDetailsPanelPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
#endif
