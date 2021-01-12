// Copyright 2020 Naotsun. All Rights Reserved.

#include "GraphPrinterUtils.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "GraphPrinterCore.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "GraphEditor.h"

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
	Options.bIsIncludeNodeInfoInImageFile = Settings->bIsIncludeNodeInfoInImageFile;
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
	TSharedPtr<SGraphEditor> GraphEditor = UGraphPrinterUtils::GetActiveGraphEditor(Options.TargetWindowOverride);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Get the range of the selected node and the position of the camera to use when drawing.
	if (!Options.bOnlySelectedNodes)
	{
		GraphEditor->SelectAllNodes();
	}

	FVector2D DrawSize;
	FVector2D ViewLocation;
	if (!FGraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(DrawSize, ViewLocation, GraphEditor, Options.Padding))
	{
		const FText& Message = FText::FromString(TEXT("No node is selected."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Erase the drawing result so that the frame for which the node is selected does not appear.
	FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	GraphEditor->ClearSelectionSet();

	// Set the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
	FVector2D PreviousViewLocation;
	float PreviousZoomAmount;
	GraphEditor->GetViewLocation(PreviousViewLocation, PreviousZoomAmount);
	GraphEditor->SetViewLocation(ViewLocation, 1.f);

	// Draw the graph editor on the render target.
	UTextureRenderTarget2D* RenderTarget = FGraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, Options.bUseGamma, Options.FilteringMode);
	
	// Restore camera position and zoom magnification.
	GraphEditor->SetViewLocation(PreviousViewLocation, PreviousZoomAmount);

	// Restore the node selection status.
	for (const auto& SelectedNode : SelectedNodes)
	{
		if (auto* GraphNode = Cast<UEdGraphNode>(SelectedNode))
		{
			GraphEditor->SetNodeSelection(GraphNode, true);
		}
	}

	if (!IsValid(RenderTarget))
	{
		const FText& Message = FText::FromString(TEXT("Failed to draw to render target."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Create output options and file path and output as image file.
	FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, FGraphPrinterCore::GetGraphTitle(GraphEditor)) +
		FGraphPrinterCore::GetImageFileExtension(Options.ImageWriteOptions.Format)
	);

	// Bind the event when the operation is completed.
	Options.ImageWriteOptions.NativeOnComplete = [Filename, Options, GraphEditor](bool bIsSucceeded)
	{
		if (bIsSucceeded)
		{
			const FText& SuccessedMessage = FText::FromString(TEXT("GraphEditor capture saved as"));
			FGraphPrinterCore::ShowNotification(
				SuccessedMessage, FGraphPrinterCore::CS_Success, 5.f, ENotificationInteraction::Hyperlink,
				FText::FromString(Filename),
				FSimpleDelegate::CreateLambda([Filename]()
			{
				FGraphPrinterCore::OpenFolderWithExplorer(Filename);
			}));

#ifdef ENABLE_EMBED_NODE_INFO
			// Embed node information in the output png image.
			if (Options.bIsIncludeNodeInfoInImageFile && (Options.ImageWriteOptions.Format == EDesiredImageFormat::PNG))
			{
				if (!FGraphPrinterCore::ExportGraphToPngFile(Filename, GraphEditor))
				{
					const FText& FailedMessage = FText::FromString(TEXT("Failed to write node information to png file."));
					FGraphPrinterCore::ShowNotification(FailedMessage, FGraphPrinterCore::CS_Fail);
				}
			}
#endif
		}
		else
		{
			const FText& FailedMessage = FText::FromString(TEXT("Failed capture GraphEditor."));
			FGraphPrinterCore::ShowNotification(FailedMessage, FGraphPrinterCore::CS_Fail);
		}
	};

	// Export the render target in the specified file format.
	FGraphPrinterCore::SaveTextureAsImageFile(RenderTarget, Filename, Options.ImageWriteOptions);
}

void UGraphPrinterUtils::RestoreNodesFromPngFile()
{
#ifdef ENABLE_EMBED_NODE_INFO
	// Get the currently active topmost window.
	TSharedPtr<SGraphEditor> GraphEditor = UGraphPrinterUtils::GetActiveGraphEditor();
	if (!GraphEditor.IsValid())
	{
		const FText& Message = FText::FromString(TEXT("The graph editor isn't currently open."));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail);
		return;
	}

	// Launch the file browser and select the png file.
	FString FilePath;
	{
		FString DefaultPath;
		if (auto* Settings = GetDefault<UGraphPrinterSettings>())
		{
			DefaultPath = Settings->OutputDirectoryPath;
		}

		TArray<FString> FilePaths;
		if (!FGraphPrinterCore::OpenFileDialog(
			FilePaths,
			TEXT("Select the png file that contains the node info"),
			DefaultPath, TEXT(""),
			TEXT("PNG Image (.png)|*.png"),
			false
		))
		{
			return;
		}

		if (!FilePaths.IsValidIndex(0))
		{
			return;
		}

		FilePath = FPaths::ConvertRelativePathToFull(FilePaths[0]);
	}

	// Restore node from png file.
	if (FGraphPrinterCore::RestoreGraphFromPngFile(FilePath, GraphEditor))
	{
		const FText& SuccessedMessage = FText::FromString(TEXT("Restore nodes from"));
		FGraphPrinterCore::ShowNotification(
			SuccessedMessage, FGraphPrinterCore::CS_Success, 5.f, ENotificationInteraction::Hyperlink,
			FText::FromString(FilePath),
			FSimpleDelegate::CreateLambda([FilePath]()
		{
			FGraphPrinterCore::OpenFolderWithExplorer(FilePath);
		}));
	}
	else
	{
		const FText& FailedMessage = FText::FromString(TEXT("Failed restore nodes."));
		FGraphPrinterCore::ShowNotification(FailedMessage, FGraphPrinterCore::CS_Fail);
	}
#endif
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

TSharedPtr<SGraphEditor> UGraphPrinterUtils::GetActiveGraphEditor(TSharedPtr<SWindow> TargetWindow /* = nullptr */)
{
	TSharedPtr<SWindow> ActiveWindow = TargetWindow;
	if (!TargetWindow.IsValid())
	{
		ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	}
	return FGraphPrinterCore::FindGraphEditor(ActiveWindow);
}
