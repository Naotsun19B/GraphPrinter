// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SWidget;
class SGraphEditor;
class UTexture;
class UTextureRenderTarget2D;
struct FImageWriteOptions;
enum class EDesiredImageFormat : uint8;

/**
 * Internal processing functions used in this plugin.
 */
class GRAPHPRINTER_API GraphPrinterCore
{
public:
	// Define notification types so you don't have to include "SNotificationList.h".
	using TCompletionState = int32;
	static const TCompletionState CS_Pending;
	static const TCompletionState CS_Success;
	static const TCompletionState CS_Fail;

public:
	// Show notifications at the bottom right of the editor.
	static bool ShowNotification(const FText NotificationText, TCompletionState CompletionState, const float ExpireDuration = 4.0f);

	// Recursively collect all child widgets of the specified widget.
	static void CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren);

	// Get SGraphEditor if it's in a child widget.
	static TSharedPtr<SGraphEditor> FindGraphEditor(TSharedPtr<SWidget> SearchTarget);

	// Draw the widget on the render target.
	static UTextureRenderTarget2D* DrawWidgetToRenderTarget(TSharedPtr<SWidget> WidgetToRender, FVector2D DrawSize, bool bUseGamma, TextureFilter Filter);

	// Saves the render target in the specified format.
	static void SaveTextureAsImageFile(UTexture* Texture, const FString& Filename, const FImageWriteOptions& Options);

	// Get the title of the graph being edited in the graph editor.
	static FString GetGraphTitle(TSharedPtr<SGraphEditor> GraphEditor);

	// Calculates the size when drawing the graph editor.
	static FVector2D CalculateGraphSize(TSharedPtr<SGraphEditor> GraphEditor, bool bSelectedNodeOnly);

	// Get the extension by the format of the image file.
	static FString GetImageFileExtension(EDesiredImageFormat ImageFormat);
};

/**
 * Cast function for classes that inherit from SWidget.
 */
namespace CastSlateWidgetPrivate
{
	template<typename To, typename From>
	TSharedPtr<To> CastSlateWidget(TSharedPtr<From> Src, const FName& DestClassName)
	{
		static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
		static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

		if (Src.IsValid())
		{
			if (Src->GetType() == DestClassName)
			{
				return StaticCastSharedPtr<To>(Src);
			}
		}

		return nullptr;
	}
}
#define CAST_SLATE_WIDGET(ToClass, FromValue) CastSlateWidgetPrivate::CastSlateWidget<ToClass>(FromValue, #ToClass)
