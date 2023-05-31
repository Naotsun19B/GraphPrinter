// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterSettings.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GenericGraphPrinter)
#endif

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPriority;
}

UPrintWidgetOptions* UGenericGraphPrinter::CreateDefaultPrintOptions(
	const UPrintWidgetOptions::EPrintScope PrintScope,
	const UPrintWidgetOptions::EExportMethod ExportMethod
) const
{
	if (UPrintWidgetOptions* PrintWidgetOptions = Super::CreateDefaultPrintOptions(PrintScope, ExportMethod))
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

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UGenericGraphPrinter::CreatePrintModeInnerPrinter( const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FGenericGraphPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UGenericGraphPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FGenericGraphPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
