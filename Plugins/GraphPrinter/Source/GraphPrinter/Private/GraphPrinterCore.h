// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "ImageWriteBlueprintLibrary.h"

class UTexture2D;
class UTextureRenderTarget2D;
enum class EDesiredImageFormat : uint8;

/**
 * Internal processing functions used in this plugin.
 */
class GRAPHPRINTER_API GraphPrinterCore
{
public:
	// Recursively collect all child widgets of the specified widget.
	static void CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren);

	// Get SGraphEditor if it's in a child widget.
	static TSharedPtr<SGraphEditor> FindGraphEditor(TSharedPtr<SWidget> SearchTarget);

	// Draw the widget on the render target.
	static UTextureRenderTarget2D* DrawWidgetToRenderTarget(TSharedPtr<SWidget> WidgetToRender, FVector2D DrawSize, bool bUseGamma, TextureFilter Filter);

	// Saves the render target in the specified format.
	static void ExportRenderTargetToDisk(UTextureRenderTarget2D* RenderTarget, const FString& Filename, const FImageWriteOptions& Options);

	// Calculates the size when drawing the graph editor.
	static bool CalculateGraphSize(TSharedPtr<SGraphEditor> GraphEditor, FVector2D& GraphSize, bool bSelectedNodeOnly);
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
