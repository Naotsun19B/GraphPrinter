// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/ActorDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

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
TOptional<GraphPrinter::FSupportedWidget> UActorDetailsPanelPrinter::CheckIfSupported(const TSharedRef<SWidget>& TestWidget) const
{
	const TSharedPtr<SActorDetails> ActorDetailsPanel = GraphPrinter::FActorDetailsPanelPrinter::FindTargetWidgetFromSearchTarget(TestWidget);
	if (!ActorDetailsPanel.IsValid())
	{
		return {};
	}

	const TSharedPtr<SDetailsView> DetailsPanel = GraphPrinter::FDetailsPanelPrinterUtils::FindNearestChildDetailsView(ActorDetailsPanel);
	if (!DetailsPanel.IsValid())
	{
		return {};
	}
	
	const FString& EditingActorName = GraphPrinter::FActorDetailsPanelPrinter::GetEditingActorName(DetailsPanel);
	return GraphPrinter::FSupportedWidget(ActorDetailsPanel.ToSharedRef(), EditingActorName, GetPriority());
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
