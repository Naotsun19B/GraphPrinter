// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/WidgetPrinters/MaterialGraphPrinter.h"
#include "MaterialGraphPrinter/WidgetPrinters/InnerMaterialGraphPrinter.h"

int32 UMaterialGraphPrinter::GetPriority() const
{
	return MaterialGraphPrinterPriority;
}

TSharedRef<GraphPrinter::IInnerPrinter> UMaterialGraphPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FMaterialGraphPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerPrinter> UMaterialGraphPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FMaterialGraphPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
