// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterSettings.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPriority;
}

UPrintWidgetOptions* UGenericGraphPrinter::CreateDefaultPrintOptions() const
{
	if (UPrintWidgetOptions* PrintWidgetOptions = Super::CreateDefaultPrintOptions())
	{
		if (auto* PrintGraphOptions = PrintWidgetOptions->Duplicate<UPrintGraphOptions>())
		{
			auto& Settings = UGenericGraphPrinterSettings::Get();
			
			PrintGraphOptions->Padding = Settings.Padding;
			PrintGraphOptions->bDrawOnlyGraph = Settings.bDrawOnlyGraph;

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
