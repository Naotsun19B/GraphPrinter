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
	TSharedPtr<SGraphEditor> GraphEditor = FGraphPrinterCore::FindGraphEditor(ActiveWindow);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Select all nodes if needed.
	if (!Options.bOnlySelectedNodes)
	{
		GraphEditor->SelectAllNodes();
	}

	// Get the range of the selected node and the position of the camera to use when drawing.
	FVector2D DrawSize;
	FVector2D ViewLocation;
	if (!FGraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(DrawSize, ViewLocation, GraphEditor, Options.Padding))
	{
		const FText& Message = FText::FromString(TEXT("No node is selected."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Set the camera position to the calculated location, draw with a zoom magnification of 1: 1 and restore it.
	FVector2D PreviousViewLocation;
	float PreviousZoomAmount;
	GraphEditor->GetViewLocation(PreviousViewLocation, PreviousZoomAmount);
	GraphEditor->SetViewLocation(ViewLocation, 1.f);
	UTextureRenderTarget2D* RenderTarget = FGraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, Options.bUseGamma, Options.FilteringMode);
	GraphEditor->SetViewLocation(PreviousViewLocation, PreviousZoomAmount);

	// Create output options and file path and output as image file.
	const FString& Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, FGraphPrinterCore::GetGraphTitle(GraphEditor)) +
		FGraphPrinterCore::GetImageFileExtension(Options.ImageWriteOptions.Format)
	);

	// Bind the event when the operation is completed.
	Options.ImageWriteOptions.NativeOnComplete = [Filename](bool bIsSucceeded)
	{
		if (bIsSucceeded)
		{
			const FText& Message = FText::FromString(TEXT("GraphEditor capture saved as"));
			FGraphPrinterCore::ShowNotification(
				Message, FGraphPrinterCore::CS_Success, 5.f, ENotificationInteraction::Hyperlink,
				FText::FromString(Filename),
				FSimpleDelegate::CreateLambda([Filename]()
			{
				FGraphPrinterCore::OpenFolderWithExplorer(Filename);
			}));
		}
		else
		{
			const FText& Message = FText::FromString(TEXT("Failed capture GraphEditor."));
			FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		}
	};

	// Export the render target in the specified file format.
	FGraphPrinterCore::SaveTextureAsImageFile(RenderTarget, Filename, Options.ImageWriteOptions);
}

void UGraphPrinterUtils::OpenExportDestinationFolder()
{
	auto* Settings = GetDefault<UGraphPrinterSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	FGraphPrinterCore::OpenFolderWithExplorer(Settings->OutputDirectoryPath);
}

void UGraphPrinterUtils::OpenFolderWithExplorer(const FString& FilePath)
{
	FGraphPrinterCore::OpenFolderWithExplorer(FilePath);
}
