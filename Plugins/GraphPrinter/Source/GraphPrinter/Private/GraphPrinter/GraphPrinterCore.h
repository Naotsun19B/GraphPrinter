// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "GraphPrinter/GraphPrinterGlobals.h"

class SWidget;
class SGraphEditor;
class STextBlock;
class SOverlay;
class SNotificationItem;
class UObject;
class UTexture;
class UTextureRenderTarget2D;
class FUICommandInfo;
struct FKeyEvent;
struct FImageWriteOptions;
struct FPrintGraphOptions;
enum class EDesiredImageFormat : uint8;

namespace GraphPrinter
{
	using FGraphPanelSelectionSet = TSet<UObject*>;

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

		// A structure for adding buttons, hyperlinks, etc. to notifications.
		struct FNotificationInteraction
		{
		public:
			// Types of things that users of editor notifications can interact with.
			enum class EInteractionType : uint8 
			{
				Hyperlink,
				Button,
			};
			EInteractionType Type;

			// The text displayed by the button or hyperlink.
			FText Text;

			// Tooltip text that appears when you hover over a button.
			FText Tooltip;

			// A callback that is called when a button or hyperlink is pressed.
			FSimpleDelegate Callback;

		public:
			// Constructor.
			FNotificationInteraction(
				EInteractionType InType,
				const FText& InText,
				const FText& InTooltip,
				const FSimpleDelegate& InCallback
			)
				: Type(InType), Text(InText), Tooltip(InTooltip), Callback(InCallback)
			{
			}
			FNotificationInteraction() : FNotificationInteraction(EInteractionType::Hyperlink, FText(), FText(), nullptr) {}
			FNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback) : FNotificationInteraction(EInteractionType::Hyperlink, InText, FText(), InCallback) {}
			FNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback) : FNotificationInteraction(EInteractionType::Button, InText, InTooltip, InCallback) {}
		};
	
	public:
		// Show notifications at the bottom right of the editor (When Expire Duration is 0 or less, you need to call the fade process manually.).
		static TSharedPtr<SNotificationItem> ShowNotification(
			const FText& NotificationText, 
			TCompletionState CompletionState, 
			float ExpireDuration = 4.f, 
			const TArray<FNotificationInteraction>& Interactions = TArray<FNotificationInteraction>()
		);

		// Recursively collect all child widgets of the specified widget.
		static void CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren);

		// Get SGraphEditor if it's in a child widget.
		static TSharedPtr<SGraphEditor> FindGraphEditor(TSharedPtr<SWidget> SearchTarget);

		// Draw the graph editor on the render target.
		// Setting the "NumberOfDrawingAttempts" too high can slow down the operation or, in the worst case, cause the engine to crash.
		static UTextureRenderTarget2D* DrawGraphToRenderTarget(
			TSharedPtr<SGraphEditor> GraphToRender, 
			const FVector2D& DrawSize, 
			bool bUseGamma, 
			TextureFilter Filter,
			bool bDrawOnlyGraph
		);

		// Enumerate all widgets that are children of SearchTarget.
		// if Predicate returns false, stop recursive process.
		static void EnumerateChildWidgets(
			TSharedPtr<SWidget> SearchTarget,
			TFunction<bool(const TSharedPtr<SWidget> ChildWidget)> Predicate
		);

		// Find the widget that is the overlay and nearest parent of SearchTarget.
		static TSharedPtr<SOverlay> FindNearestChildOverlay(TSharedPtr<SWidget> SearchTarget);
		
		// Returns the displayed text blocks that are children of SearchTarget.
		static TArray<TSharedPtr<STextBlock>> GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget);

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
}
