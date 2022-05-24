// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphPrinter
{
	/**
	 * A class that defines the function executed from the shortcut key.
	 */
	class GRAPHPRINTEREDITOREXTENSION_API FGraphPrinterCommandActions
	{
	public:
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		// Copy all nodes of the currently active graph editor as images to clipboard.
		static void CopyGraphWithAllNodesToClipboard();
		static bool CanCopyGraphWithAllNodesToClipboard();

		// Copy the selected node of the currently active graph editor as an image to clipboard.
		static void CopyGraphWithSelectedNodesToClipboard();
		static bool CanCopyGraphWithSelectedNodesToClipboard();
#endif

		// Exports all nodes of the currently active graph editor as images.
		static void PrintGraphWithAllNodes();
		static bool CanPrintGraphWithAllNodes();

		// Exports the selected node of the currently active graph editor as an image.
		static void PrintGraphWithSelectedNodes();
		static bool CanPrintGraphWithSelectedNodes();

#ifdef WITH_TEXT_CHUNK_HELPER
		// Open the file browser and restore the node from the selected png file.
		static void RestoreNodesFromPngFile();
		static bool CanExecuteRestoreWidget();
#endif

		// Open the folder containing the images saved by this plugin in explorer.
		static void OpenExportDestinationFolder();
		static bool CanOpenExportDestinationFolder();
	};
}