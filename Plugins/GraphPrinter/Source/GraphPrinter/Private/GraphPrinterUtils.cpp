// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterUtils.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "GraphPrinterCore.h"
#include "GraphPrinterTypes.h"

void UGraphPrinterUtils::PrintGraphWithAllNodes()
{
	PrintGraphFromEditorSettings(false);
}

void UGraphPrinterUtils::PrintGraphWithSelectedNodes()
{
	PrintGraphFromEditorSettings(true);
}

void UGraphPrinterUtils::PrintGraphFromEditorSettings(bool bOnlySelectedNodes, bool bIsAsync /* = true */)
{
	auto* Settings = GetDefault<UGraphPrinterSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	FPrintGraphOptions Options;
	Options.bOnlySelectedNodes = bOnlySelectedNodes;
	Options.Padding = Settings->Padding;
	Options.bUseGamma = Settings->bUseGamma;
	Options.FilteringMode = Settings->FilteringMode;
	Options.ImageWriteOptions.bAsync = bIsAsync;
	Options.ImageWriteOptions.Format = Settings->Format;
	Options.ImageWriteOptions.bOverwriteFile = Settings->bCanOverwriteFileWhenExport;
	Options.ImageWriteOptions.CompressionQuality = Settings->CompressionQuality;
	Options.OutputDirectoryPath = Settings->OutputDirectoryPath;

	CustomPrintGraph(Options);
}

void UGraphPrinterUtils::CustomPrintGraph(FPrintGraphOptions Options)
{
	// Get the currently active topmost window.
	TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (Options.TargetWindowOverride.IsValid())
	{
		ActiveWindow = Options.TargetWindowOverride;
	}
	TSharedPtr<SGraphEditor> GraphEditor = GraphPrinterCore::FindGraphEditor(ActiveWindow);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		return;
	}

	// Calculate the drawing size from the position of the node and draw on the render target.
	if (!Options.bOnlySelectedNodes)
	{
		GraphEditor->SelectAllNodes();
	}

	FVector2D DrawSize;
	FVector2D ViewLocation;
	if (!GraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(DrawSize, ViewLocation, GraphEditor, Options.Padding))
	{
		const FText& Message = FText::FromString(TEXT("No node is selected."));
		GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		return;
	}
	GraphEditor->SetViewLocation(ViewLocation, 1.f);

	UTextureRenderTarget2D* RenderTarget = GraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, Options.bUseGamma, Options.FilteringMode);

	// Create output options and file path and output as image file.
	const FString& Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, GraphPrinterCore::GetGraphTitle(GraphEditor)) + 
		GraphPrinterCore::GetImageFileExtension(Options.ImageWriteOptions.Format)
	);

	Options.ImageWriteOptions.NativeOnComplete = [Filename](bool bIsSucceeded)
	{
		if (bIsSucceeded)
		{
			const FText& Message = FText::FromString(TEXT("GraphEditor capture saved as"));
			GraphPrinterCore::ShowNotification(
				Message, GraphPrinterCore::CS_Success, 5.f, ENotificationInteraction::Hyperlink,
				FText::FromString(Filename),
				FSimpleDelegate::CreateLambda([Filename]()
			{
				GraphPrinterCore::OpenFolderWithExplorer(Filename);
			}));
		}
		else
		{
			const FText& Message = FText::FromString(TEXT("Failed capture GraphEditor."));
			GraphPrinterCore::ShowNotification(Message, GraphPrinterCore::CS_Fail);
		}
	};

	GraphPrinterCore::SaveTextureAsImageFile(RenderTarget, Filename, Options.ImageWriteOptions);
}

void UGraphPrinterUtils::OpenExportDestinationFolder()
{
	auto* Settings = GetDefault<UGraphPrinterSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	GraphPrinterCore::OpenFolderWithExplorer(Settings->OutputDirectoryPath);
}

void UGraphPrinterUtils::OpenFolderWithExplorer(const FString& FilePath)
{
	GraphPrinterCore::OpenFolderWithExplorer(FilePath);
}
