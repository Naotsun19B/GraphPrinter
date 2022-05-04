// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphPrinterEditorExtension
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

		// Copy the selected node of the currently active graph editor as an image to clipboard.
		static void CopyGraphWithSelectedNodesToClipboard();
#endif

		// Exports all nodes of the currently active graph editor as images.
		static void PrintGraphWithAllNodes();

		// Exports the selected node of the currently active graph editor as an image.
		static void PrintGraphWithSelectedNodes();

#ifdef WITH_TEXT_CHUNK_HELPER
		// Open the file browser and restore the node from the selected png file.
		static void RestoreNodesFromPngFile();
#endif

		// Open the folder containing the images saved by this plugin in Explorer.
		static void OpenExportDestinationFolder();

		// Open the Graph Printer settings screen in the editor preferences.
		static void OpenPluginSettings();
	};
}