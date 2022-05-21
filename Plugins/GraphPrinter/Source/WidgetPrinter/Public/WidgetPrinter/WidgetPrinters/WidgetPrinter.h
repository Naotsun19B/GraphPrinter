// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter/Types/OneWayBool.h"
#include "WidgetPrinter.generated.h"

// A group of parameters that must be retained for processing.
USTRUCT()
struct FWidgetPrinterParams
{
	GENERATED_BODY()

public:
	// The size of the image to output.
	UPROPERTY()
	FVector2D DrawSize;

	// A render target that holds the drawing results to be output.
	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget = nullptr;
		
	// The full path of the output file.
	UPROPERTY()
	FString Filename;
};

/**
 * A wrapper class that inherits from UObject to collect all existing printers.
 * Please implement the actual processing in the inner class.
 */
UCLASS(Abstract)
class WIDGETPRINTER_API UWidgetPrinter : public UObject
{
	GENERATED_BODY()

public:
	// Draw and export the widget with arguments.
	void PrintWidget(UPrintWidgetOptions* Options);
	
	// Returns whether the target widget can be printed.
	bool CanPrintWidget(UPrintWidgetOptions* Options);
	
	// Restore the state of the widget from the image file.
	void RestoreWidget(URestoreWidgetOptions* Options);

	// Returns whether the target widget can be restored.
	bool CanRestoreWidget(URestoreWidgetOptions* Options);
	
	// Returns the printer priority.
	// Check if the printer can be executed from the one with the highest priority.
	virtual int32 GetPriority() const;
	static int32 GetPriority(const TSubclassOf<UWidgetPrinter>& Class);

	// Generates and returns an option class with the default settings applied.
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions() const;
	virtual URestoreWidgetOptions* CreateDefaultRestoreOptions() const;
	
protected:
	// Internal processing that can be extended at the inheritance destination.
	virtual void PrintWidget();
	virtual bool CanPrintWidget();
	virtual void RestoreWidget();
	virtual bool CanRestoreWidget();
	
	// Returns the widget to print or restore.
	virtual TSharedPtr<SWidget> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const PURE_VIRTUAL(UWidgetPrinter::FindTargetWidget, { return nullptr; });

	// Called before calculating the size of the widget to draw.
	virtual void PreCalculateDrawSize() {}
	
	// Calculate the size to use when drawing the widget.
	virtual bool CalculateDrawSize(FVector2D& DrawSize);
	
	// Performs processing before draw the widget.
	virtual void PreDrawWidget() {}

	// Returns whether the widget size is drawable.
	virtual bool IsPrintableSize() const;

	// Draw the widget on the render target.
	virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget();

	// Performs processing after draw the widget.
	virtual void PostDrawWidget() {}

	// Prepare for copying to the clipboard.
	virtual void PrepareCopyToClipboard();

	// Create a file path from options.
	virtual FString CreateFilename() const;

	// Returns the title of the widget.
	virtual FString GetWidgetTitle() const PURE_VIRTUAL(UWidgetPrinter::GetWidgetTitle, { return {}; });

	// Export the render target that draws the graph editor to image file.
	virtual void ExportRenderTargetToImageFile();

	// Called when the image file export process is complete.
	virtual void OnExportRenderTargetFinished(bool bIsSucceeded);

	// Copy the image file that draws the graph editor to clipboard.
	virtual bool CopyImageFileToClipboard();

	// Write the drawn widget information to the text chunk.
	virtual bool WriteWidgetInfoToTextChunk();

	// Read information from the text chunk and restore the widget.
	virtual bool RestoreWidgetFromTextChunk();

	// Returns whether to use the optional Print Scope setting.
	virtual bool ShouldAlwaysPrintAll() const;
	
	// Get optional classes.
	UPrintWidgetOptions* GetPrintOptions() const;
	URestoreWidgetOptions* GetRestoreOptions() const;

protected:
	// The widget to draw.
	TSharedPtr<SWidget> Widget;
		
	// A group of parameters that must be retained for processing.
	UPROPERTY()
	FWidgetPrinterParams WidgetPrinterParams;
	
private:
	// Optional classes that is cached for passing to the inner class.
	UPROPERTY()
	UPrintWidgetOptions* CachedPrintOptions;
	UPROPERTY()
	URestoreWidgetOptions* CachedRestoreOptions;

protected:
	// Number of attempts to draw the widget on the render target.
	// The drawing result may be corrupted once.
	// Probably if draw twice, the drawing result will not be corrupted.
	static constexpr int32 DrawTimes = 2;

	// The number of times to re-output as a countermeasure against the whitish image
	// that is output for the first time after starting the engine.
	static constexpr int32 NumberOfReOutputWhenFirstTime = 2;
	static GraphPrinter::FOneWayBool IsFirstOutput;
};

// A macro defined at the beginning of a function when using an option class within
// a function of a class that inherits UWidgetPrinter.
#define USE_PRINT_OPTIONS_IN_THIS_FUNCTION() \
	UPrintWidgetOptions* PrintOptions = GetPrintOptions(); \
	check(IsValid(PrintOptions));

#define USE_RESTORE_OPTIONS_IN_THIS_FUNCTION() \
	URestoreWidgetOptions* RestoreOptions = GetRestoreOptions(); \
	check(IsValid(RestoreOptions));
