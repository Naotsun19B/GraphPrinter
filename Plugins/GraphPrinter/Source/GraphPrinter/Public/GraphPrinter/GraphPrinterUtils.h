// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GraphPrinter/GraphPrinterTypes.h"
#include "GraphPrinterUtils.generated.h"

class SGraphEditor;

/**
 * Make the features used in this plugin also available from Blueprint.
 */
UCLASS()
class GRAPHPRINTER_API UGraphPrinterUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Exports all nodes of the currently active graph editor as images.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void PrintGraphWithAllNodes();

	// Exports the selected node of the currently active graph editor as an image.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void PrintGraphWithSelectedNodes();

	// Copy all nodes of the currently active graph editor as images to clipboard.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void CopyGraphWithAllNodesToClipboard();

	// Copy the selected node of the currently active graph editor as an image to clipboard.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void CopyGraphWithSelectedNodesToClipboard();

	// Export the graph editor as an image based on the editor settings.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void PrintGraphFromEditorSettings(bool bOnlySelectedNodes, bool bCopyToClipboard, bool bIsAsync = true);

	// Exports graph editor as image with various options.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void CustomPrintGraph(FPrintGraphOptions Options);

	// Open the file browser and restore the node from the selected png file.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void RestoreNodesFromPngFile();
	
	// Open the folder containing the images saved by this plugin in Explorer.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void OpenExportDestinationFolder();

	// Open the folder containing the file in Explorer.
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void OpenFolderWithExplorer(const FString& Filename);

protected:
	// Get the currently active graph editor.
	// If TargetWindow is nullptr, search from MainFrame.
	static TSharedPtr<SGraphEditor> GetActiveGraphEditor(TSharedPtr<SWindow> TargetWindow = nullptr);
};
