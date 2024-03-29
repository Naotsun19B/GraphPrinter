// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/WidgetPrinters/MaterialGraphPrinter.h"
#include "MaterialGraphPrinter/WidgetPrinters/InnerMaterialGraphPrinter.h"
#include "MaterialGraphPrinter/Types/PrintMaterialGraphOptions.h"
#include "MaterialGraphPrinter/Utilities/MaterialGraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(MaterialGraphPrinter)
#endif

int32 UMaterialGraphPrinter::GetPriority() const
{
	return MaterialGraphPrinterPriority;
}

UPrintWidgetOptions* UMaterialGraphPrinter::CreateDefaultPrintOptions(
	const UPrintWidgetOptions::EPrintScope PrintScope,
	const UPrintWidgetOptions::EExportMethod ExportMethod
) const
{
	if (UPrintWidgetOptions* PrintWidgetOptions = Super::CreateDefaultPrintOptions(PrintScope, ExportMethod))
	{
		if (auto* PrintMaterialGraphOptions = PrintWidgetOptions->Duplicate<UPrintMaterialGraphOptions>())
		{
			auto& Settings = UMaterialGraphPrinterSettings::Get();
			
			PrintMaterialGraphOptions->MaterialGraphExportMethod = Settings.MaterialGraphExportMethod;

			return PrintMaterialGraphOptions;
		}
	}

	return nullptr;
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UMaterialGraphPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FMaterialGraphPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UMaterialGraphPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FMaterialGraphPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
