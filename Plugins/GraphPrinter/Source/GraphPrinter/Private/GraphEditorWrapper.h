// Copyright 2020 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterTypes.h"

class FSlateRect;
class UEdGraphNode;

/**
 * Wrapper class that wraps various types of graph editors for ease of use.
 */
class GRAPHPRINTER_API FGraphEditorWrapper
{
public:
	// Factory static function to generate this class.
	static TSharedPtr<FGraphEditorWrapper> GenerateWrapper(TSharedPtr<SGraphEditor> GraphEditor);

protected:
	// Since it is generated using the factory static function of this class, 
	// the constructor is not accessed from the outside.
	FGraphEditorWrapper(EGraphEditorType InGraphEditorType, TSharedPtr<SGraphEditor> InGraphEditor);

public:
	// Destructor.
	virtual ~FGraphEditorWrapper();

	// Get the title of the graph being edited in the graph editor.
	virtual FString GetGraphTitle() const;

	// Get number of currently selected nodes in graph editor.
	virtual int32 GetNumberOfSelectedNodes() const;

	// Exports a set of nodes to text.
	virtual bool ExportNodesToText(FString& ExportedText);

	// Imports a set of previously exported nodes into a graph.
	virtual bool ImportNodesFromText(const FString& TextToImport);

protected:
	// The type of wrapping graph editor.
	EGraphEditorType GraphEditorType;

	// Graph editor as passed in the constructor.
	TSharedPtr<SGraphEditor> GraphEditorWidget;
};
