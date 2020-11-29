// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterUtils.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "GraphPrinterCore.h"
#include "ImageWriteBlueprintLibrary.h"

void UGraphPrinterUtils::PrintGraph()
{
	auto* Settings = GetDefault<UGraphPrinterSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	// Get the currently active topmost window.
	TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	TSharedPtr<SGraphEditor> GraphEditor = GraphPrinterCore::FindGraphEditor(ActiveWindow);
	
	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		return;
	}
	
	// Calculate the drawing size from the position of the node and draw on the render target.
	GraphEditor->SelectAllNodes();

	FVector2D DrawSize;
	FVector2D ViewLocation;
	if (!GraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(DrawSize, ViewLocation, GraphEditor, 100.f))
	{
		const FText& Message = FText::FromString(TEXT("No node is selected."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		return;
	}
	GraphEditor->SetViewLocation(ViewLocation, 1.f);

	UTextureRenderTarget2D* RenderTarget = GraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, Settings->bUseGamma, Settings->FilteringMode);
	
	// Create output options and file path and output as image file.
	FImageWriteOptions Options;
	Options.bAsync = true;
	Options.Format = Settings->Format;
	Options.bOverwriteFile = Settings->bCanOverwriteFileWhenExport;
	Options.CompressionQuality = Settings->CompressionQuality;
	Options.NativeOnComplete = [](bool bIsSucceeded)
	{
		if (bIsSucceeded)
		{
			const FText& Message = FText::FromString(TEXT("Succeeded !!!"));
			GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Success);
		}
		else
		{
			const FText& Message = FText::FromString(TEXT("Failed ..."));
			GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		}
	};

	const FString& Filename = FPaths::Combine(Settings->OutputDirectoryPath, GraphPrinterCore::GetGraphTitle(GraphEditor));
	GraphPrinterCore::SaveTextureAsImageFile(RenderTarget, Filename, Options);
}
