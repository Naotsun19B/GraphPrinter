// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/ActorDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"

int32 UActorDetailsPanelPrinter::GetPriority() const
{
	return DetailsPanelPrinterPriority + 1;
}

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
