// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterCore/IGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/Utilities/GraphPrinterUtils.h"

namespace GraphPrinter
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	void FGraphPrinterCommandActions::CopyGraphWithAllNodesToClipboard()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::All;
		Options.ExportMethod = EExportMethod::Clipboard;
		IGraphPrinter::Get().PrintWidget(Options);
	}

	void FGraphPrinterCommandActions::CopyGraphWithSelectedNodesToClipboard()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::Clipboard;
		IGraphPrinter::Get().PrintWidget(Options);
	}
#endif

	void FGraphPrinterCommandActions::PrintGraphWithAllNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::All;
		Options.ExportMethod = EExportMethod::ImageFile;
		IGraphPrinter::Get().PrintWidget(Options);
	}

	void FGraphPrinterCommandActions::PrintGraphWithSelectedNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::ImageFile;
		IGraphPrinter::Get().PrintWidget(Options);
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCommandActions::RestoreNodesFromPngFile()
	{
		IGraphPrinter::Get().RestoreWidget();
	}
#endif

	void FGraphPrinterCommandActions::OpenExportDestinationFolder()
	{
		FGraphPrinterUtils::OpenFolderWithExplorer(
			UGraphPrinterSettings::Get().OutputDirectory.Path
		);
	}

	void FGraphPrinterCommandActions::OpenPluginSettings()
	{
		UGraphPrinterSettings::OpenSettings();
	}
}
