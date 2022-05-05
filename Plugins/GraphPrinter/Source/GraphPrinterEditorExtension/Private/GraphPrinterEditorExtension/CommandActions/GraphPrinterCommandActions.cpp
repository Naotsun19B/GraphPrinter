// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/Utilities/GraphPrinterUtils.h"

// #TODO: Calls the functions implemented in GraphPrinterCore module.

namespace GraphPrinter
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	void FGraphPrinterCommandActions::CopyGraphWithAllNodesToClipboard()
	{
		UE_LOG(LogTemp, Warning, TEXT("CopyGraphWithAllNodesToClipboard"));
	}

	void FGraphPrinterCommandActions::CopyGraphWithSelectedNodesToClipboard()
	{
		UE_LOG(LogTemp, Warning, TEXT("CopyGraphWithSelectedNodesToClipboard"));
	}
#endif

	void FGraphPrinterCommandActions::PrintGraphWithAllNodes()
	{
		UE_LOG(LogTemp, Warning, TEXT("PrintGraphWithAllNodes"));
	}

	void FGraphPrinterCommandActions::PrintGraphWithSelectedNodes()
	{
		UE_LOG(LogTemp, Warning, TEXT("PrintGraphWithSelectedNodes"));
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCommandActions::RestoreNodesFromPngFile()
	{
		UE_LOG(LogTemp, Warning, TEXT("RestoreNodesFromPngFile"));
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
