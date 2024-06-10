// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/DetailsPanelPrinter.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterSettings.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Types/RestoreDetailsPanelOptions.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DetailsPanelPrinter)
#endif

UDetailsPanelPrinter::UDetailsPanelPrinter()
{
#ifndef WITH_DETAILS_PANEL_PRINTER
	if (UClass* Class = GetClass())
	{
		Class->ClassFlags |= CLASS_Abstract;
	}
#endif
}

int32 UDetailsPanelPrinter::GetPriority() const
{
	return DetailsPanelPrinterPriority;
}

FString UDetailsPanelPrinter::GetSupportedWidgetTypeName() const
{
	return GraphPrinter::FDetailsPanelPrinter::GetSupportedWidgetTypeName();
}

UPrintWidgetOptions* UDetailsPanelPrinter::CreateDefaultPrintOptions(
	const UPrintWidgetOptions::EPrintScope PrintScope,
	const UPrintWidgetOptions::EExportMethod ExportMethod
) const
{
	if (UPrintWidgetOptions* PrintWidgetOptions = Super::CreateDefaultPrintOptions(PrintScope, ExportMethod))
	{
		if (auto* PrintGraphOptions = PrintWidgetOptions->Duplicate<UPrintDetailsPanelOptions>())
		{
			auto& Settings = UDetailsPanelPrinterSettings::Get();

			PrintGraphOptions->Padding = Settings.Padding;
			PrintGraphOptions->bIsIncludeExpansionStateInImageFile = Settings.bIsIncludeExpansionStateInImageFile;

			return PrintGraphOptions;
		}
	}

	return nullptr;
}

URestoreWidgetOptions* UDetailsPanelPrinter::CreateDefaultRestoreOptions() const
{
	if (URestoreWidgetOptions* RestoreWidgetOptions = Super::CreateDefaultRestoreOptions())
	{
		if (auto* RestoreDetailsPanelOptions = RestoreWidgetOptions->Duplicate<URestoreDetailsPanelOptions>())
		{
			auto& Settings = UDetailsPanelPrinterSettings::Get();
			
			RestoreDetailsPanelOptions->bWhetherToAlsoRestoreExpandedStates = Settings.bWhetherToAlsoRestoreExpandedStates;

			return RestoreDetailsPanelOptions;
		}
	}

	return nullptr;
}

#ifdef WITH_DETAILS_PANEL_PRINTER
TSharedRef<GraphPrinter::IInnerWidgetPrinter> UDetailsPanelPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FDetailsPanelPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerWidgetPrinter> UDetailsPanelPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FDetailsPanelPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}
#endif
