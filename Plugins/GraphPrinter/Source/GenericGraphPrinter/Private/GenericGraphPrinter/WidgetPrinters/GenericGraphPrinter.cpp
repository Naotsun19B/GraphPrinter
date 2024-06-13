// Copyright 2020-2024 Naotsun. All Rights Reserved.

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

TOptional<GraphPrinter::FSupportedWidget> UGenericGraphPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GraphPrinter::FGenericGraphPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!GraphEditor.IsValid())
	{
		return {};
	}

	FString GraphTitle;
	if (!GraphPrinter::FGenericGraphPrinter::GetGraphTitle(GraphEditor, GraphTitle))
	{
		return {};
	}
	
	return GraphPrinter::FSupportedWidget(GraphEditor.ToSharedRef(), GraphTitle, GetPriority());
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
