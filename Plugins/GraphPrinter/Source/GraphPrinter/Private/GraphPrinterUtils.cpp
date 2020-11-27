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
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		return;
	}
	
	UTextureRenderTarget2D* RenderTarget = GraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, false, TF_Default);
	
	const FString& Filename = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Test"));
	if (GraphPrinterCore::ExportRenderTargetToDisk(RenderTarget, Filename, EDesiredImageFormat::PNG))
	{
		const FText& Message = FText::FromString(TEXT("Successed !!!"));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Success);
	}
	else
	{
		const FText& Message = FText::FromString(TEXT("Failed ..."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
	}
}
