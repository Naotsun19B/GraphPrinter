// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterTypes.h"

class SWidget;
class SGraphEditor;
class SNotificationItem;
class UTexture;
class UTextureRenderTarget2D;
struct FImageWriteOptions;
typedef TSet<class UObject*> FGraphPanelSelectionSet;
enum class EDesiredImageFormat : uint8;

/**
 * Internal processing functions used in this plugin.
 */
class GRAPHPRINTER_API FGraphPrinterCore
{
public:
	// Define notification types so don't need to include "SNotificationList.h".
	using TCompletionState = int32;
	static const TCompletionState CS_Pending;
	static const TCompletionState CS_Success;
	static const TCompletionState CS_Fail;

public:
	// Show notifications at the bottom right of the editor (When Expire Duration is 0 or less, you need to call the fade process manually.).
	static TSharedPtr<SNotificationItem> ShowNotification(
		const FText& NotificationText, 
		TCompletionState CompletionState, 
		float ExpireDuration = 4.f, 
		ENotificationInteraction InteractionType = ENotificationInteraction::None,
		const FText& InteractionText = FText(),
		FSimpleDelegate InteractionCallback = nullptr
	);

	// Recursively collect all child widgets of the specified widget.
	static void CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren);

	// Get SGraphEditor if it's in a child widget.
	static TSharedPtr<SGraphEditor> FindGraphEditor(TSharedPtr<SWidget> SearchTarget);

	// Draw the widget on the render target.
	// Setting the "NumberOfDrawingAttempts" too high can slow down the operation or, in the worst case, cause the engine to crash.
	static UTextureRenderTarget2D* DrawWidgetToRenderTarget(
		TSharedPtr<SWidget> WidgetToRender, 
		const FVector2D& DrawSize, 
		bool bUseGamma, 
		TextureFilter Filter
	);

	// Create a file path from options.
	static FString CreateFilename(TSharedPtr<SGraphEditor> GraphEditor, const FPrintGraphOptions& Options);

	// Calculate the range and view location to use when drawing the graph editor.
	static bool CalculateGraphDrawSizeAndViewLocation(
		FVector2D& DrawSize, 
		FVector2D& ViewLocation, 
		TSharedPtr<SGraphEditor> GraphEditor, 
		float Padding
	);

	// Get the title of the graph being edited in the graph editor.
	static FString GetGraphTitle(TSharedPtr<SGraphEditor> GraphEditor);

	// Get the extension by the format of the image file.
	static FString GetImageFileExtension(EDesiredImageFormat ImageFormat);

	// Open the folder containing the file in Explorer.
	static void OpenFolderWithExplorer(const FString& Filename);

#if ENABLE_EMBED_NODE_INFO
	// Writes the information of the node selected in Graph Editor to the png file.
	static bool ExportGraphToPngFile(const FString& Filename, TSharedPtr<SGraphEditor> GraphEditor, const FGraphPanelSelectionSet& NodesToExport);

	// Restore the node from the information read from the png file.
	static bool RestoreGraphFromPngFile(const FString& Filename, TSharedPtr<SGraphEditor> GraphEditor);

	// Show the file browser and let the user select a file.
	static bool OpenFileDialog(
		TArray<FString>& Filenames,
		const FString& DialogTitle = TEXT("Open File Dialog"),
		const FString& DefaultPath = TEXT(""),
		const FString& DefaultFile = TEXT(""),
		const FString& FileTypes = TEXT("All (*)|*.*"),
		bool bIsMultiple = false
	);

	// Create an FKeyEvent from FUICommandInfo.
	static bool GetKeyEventFromUICommandInfo(const TSharedPtr<FUICommandInfo>& UICommandInfo, FKeyEvent& OutKeyEvent);
#endif
};

/**
 * Cast function for classes that inherit from SWidget.
 */
namespace CastSlateWidgetPrivate
{
	template<class To, class From>
	TSharedPtr<To> CastSlateWidget(TSharedPtr<From> FromPtr, const FName& ToClassName)
	{
		static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
		static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

		if (FromPtr.IsValid())
		{
			if (FromPtr->GetType() == ToClassName)
			{
				return StaticCastSharedPtr<To>(FromPtr);
			}
		}

		return nullptr;
	}
}
#define CAST_SLATE_WIDGET(ToClass, FromPtr) CastSlateWidgetPrivate::CastSlateWidget<ToClass>(FromPtr, #ToClass)
