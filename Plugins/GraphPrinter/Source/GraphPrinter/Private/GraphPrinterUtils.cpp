// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinterUtils.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "GraphPrinterCore.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "GraphEditor.h"

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinterUtilsInternal
{
	// The number of times to re-output as a countermeasure against the whitish image
	// that is output for the first time after starting the engine.
	static const int32 NumberOfReOutputWhenFirstTime = 2;
}

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
	Options.bUseGamma = Settings->bUseGamma;
	Options.Padding = Settings->Padding;
	Options.MaxImageSize = Settings->MaxImageSize;
	Options.FilteringMode = Settings->FilteringMode;
	Options.bIsIncludeNodeInfoInImageFile = Settings->bIsIncludeNodeInfoInImageFile;
	Options.ImageWriteOptions.bAsync = bIsAsync;
	Options.ImageWriteOptions.Format = Settings->Format;
	Options.ImageWriteOptions.bOverwriteFile = Settings->bCanOverwriteFileWhenExport;
	Options.ImageWriteOptions.CompressionQuality = Settings->CompressionQuality;
	Options.OutputDirectoryPath = Settings->OutputDirectory.Path;

	CustomPrintGraph(Options);
}

void UGraphPrinterUtils::CustomPrintGraph(FPrintGraphOptions Options)
{
	// Get the currently active topmost window.
	TSharedPtr<SGraphEditor> GraphEditor = UGraphPrinterUtils::GetActiveGraphEditor(Options.TargetWindowOverride);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("GraphOpenError", "The graph editor isn't currently open."),
			FGraphPrinterCore::CS_Fail
		);
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
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("NotSelectedError", "No node is selected."),
			FGraphPrinterCore::CS_Fail
		);
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

	// Check draw size.
	bool bIsBelowMaxDrawSize = true;
	if (Options.MaxImageSize > FVector2D::ZeroVector)
	{
		if (DrawSize.X > Options.MaxImageSize.X || DrawSize.Y > Options.MaxImageSize.Y)
		{
			bIsBelowMaxDrawSize = false;
		}
	}

	// Draw the graph editor on the render target.
	UTextureRenderTarget2D* RenderTarget = nullptr;
	if (bIsBelowMaxDrawSize)
	{
		RenderTarget = FGraphPrinterCore::DrawWidgetToRenderTarget(GraphEditor, DrawSize, Options.bUseGamma, Options.FilteringMode);
	}

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

	if (!bIsBelowMaxDrawSize)
	{
		const FText& Message = FText::FromString(FString::Printf(
			TEXT("%s / %s\nThe drawing range is too wide.\nIf necessary, change the maximum size from the editor settings."),
			*DrawSize.ToString(), *Options.MaxImageSize.ToString()
		));
		FGraphPrinterCore::ShowNotification(Message, FGraphPrinterCore::CS_Fail, 6.f);
		return;
	}

	if (!IsValid(RenderTarget))
	{
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("DrawError", "Failed to draw to render target."),
			FGraphPrinterCore::CS_Fail
		);
		return;
	}

	// Create output options and file path and output as image file.
	FString Filename = FGraphPrinterCore::CreateFilename(GraphEditor, Options);

	// Bind the event when the operation is completed.
	Options.ImageWriteOptions.NativeOnComplete = [Filename, Options, GraphEditor, SelectedNodes](bool bIsSucceeded)
	{
		if (bIsSucceeded)
		{
			FGraphPrinterCore::ShowNotification(
				LOCTEXT("SuccessedOutput", "GraphEditor capture saved as"),
				FGraphPrinterCore::CS_Success, 5.f,
				TArray<FGraphPrinterCore::FNotificationInteraction>{
					FGraphPrinterCore::FNotificationInteraction(
						FText::FromString(Filename),
						FSimpleDelegate::CreateLambda([Filename]()
						{
							FGraphPrinterCore::OpenFolderWithExplorer(Filename);
						})
					)
				}
			);

#if ENABLE_EMBED_NODE_INFO
			// Embed node information in the output png image.
			if (Options.bIsIncludeNodeInfoInImageFile && (Options.ImageWriteOptions.Format == EDesiredImageFormat::PNG))
			{
				if (!FGraphPrinterCore::ExportGraphToPngFile(Filename, GraphEditor, SelectedNodes))
				{
					FGraphPrinterCore::ShowNotification(
						LOCTEXT("FailedEmbedNodeInfoError", "Failed to write node information to png file."),
						FGraphPrinterCore::CS_Fail
					);
				}
			}
#endif
		}
		else
		{
			FGraphPrinterCore::ShowNotification(
				LOCTEXT("FailedOutputError", "Failed capture GraphEditor."),
				FGraphPrinterCore::CS_Fail
			);
		}
	};

	// Export the render target in the specified file format.
	UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, Options.ImageWriteOptions);

	// As a symptomatic treatment for the problem that the first image output after startup is whitish,
	// the first output is re-output as many times as NumberOfRedrawsWhenFirstTime.
	static bool bDoOnce = true;
	if (bDoOnce)
	{
		for (int32 Count = 0; Count <GraphPrinterUtilsInternal::NumberOfReOutputWhenFirstTime; Count++)
		{
			FImageWriteOptions ImageWriteOptions = Options.ImageWriteOptions;
			ImageWriteOptions.bOverwriteFile = true;
			ImageWriteOptions.NativeOnComplete = nullptr;
			UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, ImageWriteOptions);
			bDoOnce = false;
		}	
	}
}

void UGraphPrinterUtils::RestoreNodesFromPngFile()
{
#if ENABLE_EMBED_NODE_INFO
	// Get the currently active topmost window.
	const TSharedPtr<SGraphEditor> GraphEditor = GetActiveGraphEditor();
	if (!GraphEditor.IsValid())
	{
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("NotFoundGraphEditorError", "The graph editor isn't currently open."),
			FGraphPrinterCore::CS_Fail
		);
		return;
	}

	// Launch the file browser and select the png file.
	FString Filename;
	{
		FString DefaultPath;
		if (auto* Settings = GetDefault<UGraphPrinterSettings>())
		{
			DefaultPath = Settings->OutputDirectory.Path;
		}

		TArray<FString> Filenames;
		if (!FGraphPrinterCore::OpenFileDialog(
			Filenames,
			TEXT("Select the png file that contains the node info"),
			DefaultPath, TEXT(""),
			TEXT("PNG Image (.png)|*.png"),
			false
		))
		{
			return;
		}

		if (!Filenames.IsValidIndex(0))
		{
			return;
		}

		Filename = FPaths::ConvertRelativePathToFull(Filenames[0]);
	}

	// Restore node from png file.
	if (FGraphPrinterCore::RestoreGraphFromPngFile(Filename, GraphEditor))
	{
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("SuccessedRestore", "Restore nodes from"),
			FGraphPrinterCore::CS_Success, 5.f,
			TArray<FGraphPrinterCore::FNotificationInteraction>{
				FGraphPrinterCore::FNotificationInteraction(
					FText::FromString(Filename),
					FSimpleDelegate::CreateLambda([Filename]()
					{
						FGraphPrinterCore::OpenFolderWithExplorer(Filename);
					})
				)
			}
		);
	}
	else
	{
		FGraphPrinterCore::ShowNotification(
			LOCTEXT("FailedRestoreError", "Failed restore nodes."),
			FGraphPrinterCore::CS_Fail
		);
	}
#else
	UE_LOG(LogGraphPrinter, Warning, TEXT("This function cannot be used because the function to embed node information in a png image is disabled.\nSee GraphPrinter.Build.cs to enable it."));
#endif
}

void UGraphPrinterUtils::OpenExportDestinationFolder()
{
	auto* Settings = GetDefault<UGraphPrinterSettings>();
	if (!IsValid(Settings))
	{
		return;
	}

	FGraphPrinterCore::OpenFolderWithExplorer(Settings->OutputDirectory.Path);
}

void UGraphPrinterUtils::OpenFolderWithExplorer(const FString& Filename)
{
	FGraphPrinterCore::OpenFolderWithExplorer(Filename);
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

#undef LOCTEXT_NAMESPACE
