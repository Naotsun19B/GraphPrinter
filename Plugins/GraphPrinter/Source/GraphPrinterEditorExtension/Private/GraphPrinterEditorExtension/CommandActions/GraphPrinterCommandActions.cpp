// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "GraphPrinter/IGraphPrinter.h"
#include "GraphPrinter/Utilities/GraphPrinterSettings.h"
#include "Misc/Paths.h"

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

	bool FGraphPrinterCommandActions::CanCopyGraphWithAllNodesToClipboard()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::All;
		Options.ExportMethod = EExportMethod::Clipboard;
		return IGraphPrinter::Get().CanPrintWidget(Options);
	}

	void FGraphPrinterCommandActions::CopyGraphWithSelectedNodesToClipboard()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::Clipboard;
		IGraphPrinter::Get().PrintWidget(Options);
	}

	bool FGraphPrinterCommandActions::CanCopyGraphWithSelectedNodesToClipboard()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::Clipboard;
		return IGraphPrinter::Get().CanPrintWidget(Options);
	}
#endif

	void FGraphPrinterCommandActions::PrintGraphWithAllNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::All;
		Options.ExportMethod = EExportMethod::ImageFile;
		IGraphPrinter::Get().PrintWidget(Options);
	}

	bool FGraphPrinterCommandActions::CanPrintGraphWithAllNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::All;
		Options.ExportMethod = EExportMethod::ImageFile;
		return IGraphPrinter::Get().CanPrintWidget(Options);
	}

	void FGraphPrinterCommandActions::PrintGraphWithSelectedNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::ImageFile;
		IGraphPrinter::Get().PrintWidget(Options);
	}

	bool FGraphPrinterCommandActions::CanPrintGraphWithSelectedNodes()
	{
		FPrintWidgetOptions Options = UGraphPrinterSettings::Get().GeneratePrintGraphOptions();
		Options.PrintScope = EPrintScope::Selected;
		Options.ExportMethod = EExportMethod::ImageFile;
		return IGraphPrinter::Get().CanPrintWidget(Options);
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCommandActions::RestoreNodesFromPngFile()
	{
		IGraphPrinter::Get().RestoreWidget();
	}

	bool FGraphPrinterCommandActions::CanExecuteRestoreWidget()
	{
		return IGraphPrinter::Get().CanRestoreWidget();
	}
#endif

	void FGraphPrinterCommandActions::OpenExportDestinationFolder()
	{
		FGraphPrinterUtils::OpenFolderWithExplorer(
			UGraphPrinterSettings::Get().OutputDirectory.Path
		);
	}

	bool FGraphPrinterCommandActions::CanOpenExportDestinationFolder()
	{
		return FPaths::DirectoryExists(UGraphPrinterSettings::Get().OutputDirectory.Path);
	}

	void FGraphPrinterCommandActions::OpenPluginSettings()
	{
		UGraphPrinterSettings::OpenSettings();
	}
}
