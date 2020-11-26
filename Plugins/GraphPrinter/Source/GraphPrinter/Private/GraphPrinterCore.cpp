// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCore.h"
#include "GraphPrinterGlobals.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"
#include "GraphEditor.h"

void GraphPrinterCore::CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren)
{
	if (SearchTarget != nullptr)
	{
		if (FChildren* Children = SearchTarget->GetAllChildren())
		{
			for (int32 Index = 0; Index < Children->Num(); Index++)
			{
				TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
				OutChildren.Add(ChildWidget);
				CollectAllChildWidgets(ChildWidget, OutChildren);
			}
		}
	}
}

TSharedPtr<SGraphEditor> GraphPrinterCore::FindGraphEditor(TSharedPtr<SWidget> SearchTarget)
{
	TArray<TSharedPtr<SWidget>> ChildWidgets;
	CollectAllChildWidgets(SearchTarget, ChildWidgets);

	for (const auto& ChildWidget : ChildWidgets)
	{
		TSharedPtr<SGraphEditor> GraphEditor = CAST_SLATE_WIDGET(SGraphEditor, ChildWidget);
		if (GraphEditor.IsValid())
		{
			return GraphEditor;
		}
	}

	return nullptr;
}

UTextureRenderTarget2D* GraphPrinterCore::DrawWidgetToRenderTarget(TSharedPtr<SWidget> WidgetToRender, FVector2D DrawSize, bool bUseGamma, TextureFilter Filter)
{
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGamma);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}

	WidgetRenderer->DrawWidget(RenderTarget, WidgetToRender.ToSharedRef(), DrawSize, 0.f, false);
	FlushRenderingCommands();
	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

void GraphPrinterCore::ExportRenderTargetToDisk(UTextureRenderTarget2D* RenderTarget, const FString& Filename, const FImageWriteOptions& Options)
{
	// TODO : The following method will result in an error, so look for another method.

	UTexture2D* TextureToExport = UTexture2D::CreateTransient(RenderTarget->SizeX, RenderTarget->SizeY, RenderTarget->GetFormat());
	if (IsValid(RenderTarget))
	{
		UTexture2D* NewTexture = RenderTarget->ConstructTexture2D(TextureToExport->GetOuter(), TextureToExport->GetName(), RenderTarget->GetMaskedFlags() | RF_Public | RF_Standalone, CTF_Default, NULL);
		if (IsValid(NewTexture))
		{
			NewTexture->Modify();
			NewTexture->MarkPackageDirty();
			NewTexture->PostEditChange();
			NewTexture->UpdateResource();
		}
	}
	UImageWriteBlueprintLibrary::ExportToDisk(TextureToExport, Filename, Options);
}

bool GraphPrinterCore::CalculateGraphSize(TSharedPtr<SGraphEditor> GraphEditor, FVector2D& GraphSize, bool bSelectedNodeOnly)
{
	GraphSize = FVector2D(1920, 1080);
	return GraphEditor.IsValid();
}
