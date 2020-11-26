// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterUtils.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterCore.h"
#include "Misc/Paths.h"

void UGraphPrinterUtils::PrintGraph()
{
	UE_LOG(LogGraphPrinter, Log, TEXT("Called UGraphPrinterUtils::PrintGraph"));

	TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	TSharedPtr<SGraphEditor> GraphEditor = GraphPrinterCore::FindGraphEditor(ActiveWindow);
	
	FVector2D DrawSize;
	if (!GraphPrinterCore::CalculateGraphSize(GraphEditor, DrawSize, false))
	{
		return;
	}
	
	UTextureRenderTarget2D* RenderTarget = GraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, false, TF_Default);
	
	FImageWriteOptions Options;
	Options.Format = EDesiredImageFormat::PNG;
	Options.NativeOnComplete = [](bool bIsSucceed)
	{
		if (bIsSucceed)
		{
			UE_LOG(LogGraphPrinter, Log, TEXT("PrintGraph Succeed !!!"));
		}
		else
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("PrintGraph Failed..."));
		}
	};
	GraphPrinterCore::ExportRenderTargetToDisk(RenderTarget, FPaths::ProjectSavedDir(), Options);
}
