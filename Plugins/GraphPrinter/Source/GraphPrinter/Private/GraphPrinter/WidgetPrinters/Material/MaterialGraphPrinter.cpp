// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/Material/MaterialGraphPrinter.h"
#include "SGraphEditorImpl.h"
#include "MaterialGraph/MaterialGraph.h"

bool UMaterialGraphPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	if (GraphEditor.IsValid())
	{
		return IsValid(Cast<UMaterialGraph>(GraphEditor->GetCurrentGraph()));
	}

	return false;
}

int32 UMaterialGraphPrinter::GetPriority() const
{
	return MaterialGraphPrinterPrinter;
}
