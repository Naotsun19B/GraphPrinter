// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterTypes.h"

class SWidget;
class SGraphEditor;
class SNotificationItem;
class UTexture;
class UTextureRenderTarget2D;
struct FImageWriteOptions;
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

	// Get the editor world without context.
	static UWorld* GetEditorWorld();

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

	// Saves the render target in the specified format.
	static void SaveTextureAsImageFile(UTexture* Texture, const FString& Filename, const FImageWriteOptions& Options);

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
	static void OpenFolderWithExplorer(const FString& FilePath);
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
