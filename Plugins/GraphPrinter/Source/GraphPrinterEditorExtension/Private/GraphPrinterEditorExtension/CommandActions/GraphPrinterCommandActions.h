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
		// Copy the entire target widget as an image to the clipboard.
		static void CopyAllAreaOfWidgetToClipboard();
		static bool CanCopyAllAreaOfWidgetToClipboard();

		// Copy the selected area of the target widget to the clipboard as an image.
		static void CopySelectedAreaOfWidgetToClipboard();
		static bool CanCopySelectedAreaOfWidgetToClipboard();
#endif

		// Outputs the entire target widget as an image file.
		static void PrintAllAreaOfWidget();
		static bool CanPrintAllAreaOfWidget();

		// Outputs the selected area of the target widget as an image file.
		static void PrintSelectedAreaOfWidget();
		static bool CanPrintSelectedAreaOfWidget();

#ifdef WITH_TEXT_CHUNK_HELPER
		// Restores the state of the widget from the image file.
		// You can only restore from images output from this plugin.
		static void RestoreWidgetFromImageFile();
		static bool CanExecuteRestoreWidget();
#endif

		// Open the folder containing the images saved by this plugin in explorer.
		static void OpenExportDestinationFolder();
		static bool CanOpenExportDestinationFolder();
	};
}