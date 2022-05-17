// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinter/WidgetPrinters/WidgetPrinter.h"
#include "GraphEditor.h"
#include "GenericGraphPrinter.generated.h"

class SGraphEditorImpl;
class UTextureRenderTarget2D;

/**
 * A general-purpose printer class for the graph editor.
 */
UCLASS()
class GRAPHPRINTER_API UGenericGraphPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 GenericGraphPrinterPrinter = 0;
	
public:
	// UWidgetPrinter interface.
	virtual void PrintWidget(GraphPrinter::FPrintWidgetOptions Options) override;
	virtual bool CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const override;
#ifdef WITH_TEXT_CHUNK_HELPER
	virtual void RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options) override;
	virtual bool CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const override;
#endif
	virtual int32 GetPriority() const override;
	virtual FString GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const override;
	// End of UWidgetPrinter interface.

protected:
	// Returns the graph editor to print or restore.
	virtual TSharedPtr<SGraphEditorImpl> FindGraphEditor(const TSharedPtr<SWidget>& TargetWidget) const;
	
	// Calculate the range and view location to use when drawing the graph editor.
	virtual bool CalculateGraphDrawSizeAndViewLocation(
		FVector2D& DrawSize, 
		FVector2D& ViewLocation, 
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const GraphPrinter::FPrintWidgetOptions& Options
	) const;

	// Performs processing before shooting the graph editor.
	virtual void PrePrintGraphEditor(
		FVector2D& PreviousViewLocation, 
		float& PreviousZoomAmount,
		FGraphPanelSelectionSet& NodesToPrint,
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const FVector2D& DrawSize,
		const FVector2D& ViewLocation,
		const GraphPrinter::FPrintWidgetOptions& Options
	);
	
	// Draw the graph editor on the render target.
	virtual UTextureRenderTarget2D* DrawGraphToRenderTarget(
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const FVector2D& DrawSize,
		const FVector2D& ViewLocation,
		const GraphPrinter::FPrintWidgetOptions& Options
	);
	
	// Performs processing after shooting the graph editor.
	virtual void PostPrintGraphEditor(
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const FVector2D& PreviousViewLocation, 
		float PreviousZoomAmount,
		const FGraphPanelSelectionSet& PreviousSelectedNodes,
		const GraphPrinter::FPrintWidgetOptions& Options
	);

#ifdef WITH_TEXT_CHUNK_HELPER
	// Write the drawn node information to the text chunk.
	virtual bool WriteNodeInfoToTextChunk(
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const FString& Filename,
		const FGraphPanelSelectionSet& NodesToPrint,
		const GraphPrinter::FPrintWidgetOptions& Options
	);

	// Read information from the text chunk and restore the node.
	virtual bool RestoreNodesFromTextChunk(
		const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
		const FString& Filename,
		const GraphPrinter::FRestoreWidgetOptions& Options
	);
#endif
	
protected:
#ifdef WITH_TEXT_CHUNK_HELPER
	// Key used when writing to a text chunk of a png file.
	static const FString PngTextChunkKey;

	// The beginning of the node information.
	static const FString NodeInfoHeader;
#endif
};
