// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/ActorDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(ActorDetailsPanelPrinter)
#endif

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
FString UActorDetailsPanelPrinter::GetSupportedWidgetTypeName() const
{
	return GraphPrinter::FActorDetailsPanelPrinter::GetSupportedWidgetTypeName();
}

FText UActorDetailsPanelPrinter::GetWidgetDisplayName(const TSharedRef<SWidget>& Widget) const
{
	const TSharedPtr<SActorDetails> ActorDetailsPanel = GP_CAST_SLATE_WIDGET(SActorDetails, TSharedPtr<SWidget>(Widget));
	const TSharedPtr<SDetailsView> DetailsPanel = GraphPrinter::FDetailsPanelPrinterUtils::FindNearestChildDetailsView(ActorDetailsPanel);
	return FText::FromString(GraphPrinter::FActorDetailsPanelPrinter::GetEditingActorName(DetailsPanel));
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
#endif
