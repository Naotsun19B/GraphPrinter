// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "GenericGraphPrinter.generated.h"

/**
 * A general-purpose printer class for the graph editor.
 */
UCLASS()
class GENERICGRAPHPRINTER_API UGenericGraphPrinter : public UWidgetPrinter
{
	GENERATED_BODY()

public:
	// The priority of this widget printer.
	static constexpr int32 GenericGraphPrinterPrinter = 0;
	
public:
	// UWidgetPrinter interface.
	virtual int32 GetPriority() const override;
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions() const override;
	virtual TSharedPtr<SWidget> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override;
	virtual void PreCalculateDrawSize() override;
	virtual bool CalculateDrawSize(FVector2D& DrawSize) override;
	virtual void PreDrawWidget() override;
	virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override;
	virtual void PostDrawWidget() override;
	virtual FString GetWidgetTitle() const override;
	virtual bool WriteWidgetInfoToTextChunk() override;
	virtual bool RestoreWidgetFromTextChunk() override;
	virtual bool ShouldAlwaysPrintAll() const override;
	// End of UWidgetPrinter interface.

protected:
	// Calculate the range and view location to use when drawing the graph editor.
	virtual bool CalculateGraphDrawSizeAndViewLocation(FVector2D& DrawSize, FVector2D& ViewLocation);

protected:
	// A group of parameters that must be retained for processing.
	struct FGenericGraphPrinterParams
	{
		// In order to operate the camera position and zoom magnification for drawing, keep the previous state and restore it.
		FGraphPanelSelectionSet PreviousSelectedNodes;
		FVector2D PreviousViewLocation;
		float PreviousZoomAmount;

		// The nodes to be drawn.
		FGraphPanelSelectionSet NodesToPrint;
			
		// The size of the image to output.
		FVector2D ViewLocation;
	};
	FGenericGraphPrinterParams GenericGraphPrinterParams;

#ifdef WITH_TEXT_CHUNK_HELPER
protected:
	// Key used when writing to a text chunk of a png file.
	static const FString PngTextChunkKey;

	// The beginning of the node information.
	static const FString NodeInfoHeader;
#endif
};
