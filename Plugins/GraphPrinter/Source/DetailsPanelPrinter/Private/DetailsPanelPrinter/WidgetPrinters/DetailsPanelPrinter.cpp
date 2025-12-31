// Copyright 2020-2026 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/DetailsPanelPrinter.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterSettings.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Types/RestoreDetailsPanelOptions.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(DetailsPanelPrinter)
#endif

int32 UDetailsPanelPrinter::GetPriority() const
{
	return DetailsPanelPrinterPriority;
}

TOptional<GraphPrinter::FSupportedWidget> UDetailsPanelPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SDetailsView> DetailsPanel = GraphPrinter::FDetailsPanelPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!DetailsPanel.IsValid())
	{
		return {};
	}

	const FString& EditingObjectName = GraphPrinter::FDetailsPanelPrinter::GetEditingObjectName(DetailsPanel);
	return GraphPrinter::FSupportedWidget(DetailsPanel.ToSharedRef(), EditingObjectName, GetPriority());
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
			const auto& Settings = GraphPrinter::GetSettings<UDetailsPanelPrinterSettings>();

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
			const auto& Settings = GraphPrinter::GetSettings<UDetailsPanelPrinterSettings>();
			
			RestoreDetailsPanelOptions->bWhetherToAlsoRestoreExpandedStates = Settings.bWhetherToAlsoRestoreExpandedStates;

			return RestoreDetailsPanelOptions;
		}
	}

	return nullptr;
}

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
