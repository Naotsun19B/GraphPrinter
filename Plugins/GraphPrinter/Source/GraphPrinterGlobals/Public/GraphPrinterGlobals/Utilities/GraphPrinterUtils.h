// Copyright 2020-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

namespace GraphPrinter
{
	/**
	 * A utility class that defines utility functions used in this plugin.
	 */
	class GRAPHPRINTERGLOBALS_API FGraphPrinterUtils
	{
	public:
		// Gets the extension by the format of the image file.
		static FString GetImageFileExtension(const EDesiredImageFormat ImageFormat, const bool bWithDot = true);
		
		// Opens the folder containing the file in Explorer.
		static void OpenFolderWithExplorer(const FString& Filename);

		// Shows the file browser and let the user select a file.
		static bool OpenFileDialog(
			TArray<FString>& Filenames,
			const FString& DialogTitle = TEXT("Open File Dialog"),
			const FString& DefaultPath = TEXT(""),
			const FString& DefaultFile = TEXT(""),
			const FString& FileTypes = TEXT("All (*)|*.*"),
			const bool bIsMultiple = false
		);

		// Cuts out unnecessary character strings mixed in at the beginning of sentences.
		// Returns whether the clipping was actually done.
		static bool ClearUnnecessaryCharactersFromHead(FString& String, const FString& BeginningOfString);
	};
}
