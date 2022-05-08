// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/WidgetPrinters/GenericGraphPrinter.h"
#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/Utilities/GraphPrinterUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "SGraphEditorImpl.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"
#include "HAL/FileManager.h"
#include "Framework/Commands/GenericCommands.h"
#include "EdGraphUtilities.h"

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
#include "ClipboardImageExtension/HAL/ClipboardImageExtension.h"
#endif

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

#define LOCTEXT_NAMESPACE "GenericGraphPrinter"

void UGenericGraphPrinter::PrintWidget(GraphPrinter::FPrintWidgetOptions Options)
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("GraphOpenError", "The graph editor isn't currently open."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	// Holds the node selected by the user for printing and then restoring.
	FGraphPanelSelectionSet PreviousSelectedNodes = GraphEditor->GetSelectedNodes();

	// Get the range of the selected node and the position of the camera to use when drawing.
	if (Options.PrintScope == GraphPrinter::EPrintScope::All)
	{
		GraphEditor->SelectAllNodes();
	}

	FVector2D DrawSize;
	FVector2D ViewLocation;
	if (!CalculateGraphDrawSizeAndViewLocation(DrawSize, ViewLocation, GraphEditor, Options))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotSelectedError", "No node is selected."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	FVector2D PreviousViewLocation;
	float PreviousZoomAmount;
	FGraphPanelSelectionSet NodesToPrint;
	PrePrintGraphEditor(
		PreviousViewLocation,
		PreviousZoomAmount,
		NodesToPrint,
		GraphEditor,
		DrawSize,
		ViewLocation,
		Options
	);

	const bool bIsBelowMaxDrawSize = CanPrintGraphEditor(GraphEditor, DrawSize, ViewLocation, Options);

	// Draw the graph editor on the render target.
	UTextureRenderTarget2D* RenderTarget = nullptr;
	if (bIsBelowMaxDrawSize)
	{
		RenderTarget = DrawGraphToRenderTarget(GraphEditor, DrawSize, ViewLocation, Options);
	}

	PostPrintGraphEditor(
		GraphEditor,
		PreviousViewLocation,
		PreviousZoomAmount,
		PreviousSelectedNodes,
		Options
	);

	if (!bIsBelowMaxDrawSize)
	{
		const FText& Message = FText::FromString(FString::Printf(
			TEXT("%s / %s\nThe drawing range is too wide.\nIf necessary, change the maximum size from the editor settings."),
			*DrawSize.ToString(), *Options.MaxImageSize.ToString()
		));
		GraphPrinter::FGraphPrinterUtils::ShowNotification(Message, GraphPrinter::FGraphPrinterUtils::CS_Fail, 6.f);
		return;
	}

	if (!IsValid(RenderTarget))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("DrawError", "Failed to draw to render target."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	PrepareCopyToClipboard(Options);
#endif

	// Create output options and file path and output as image file.
	FString Filename = CreateFilename(GraphEditor, Options);

	// Bind the event when the operation is completed.
	TWeakObjectPtr<UGenericGraphPrinter> WeakThis = this;
	Options.ImageWriteOptions.NativeOnComplete = [WeakThis, GraphEditor, Filename, NodesToPrint, Options](bool bIsSucceeded)
	{
		if (!WeakThis.IsValid() || !bIsSucceeded)
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("FailedOutputError", "Failed capture GraphEditor."),
				GraphPrinter::FGraphPrinterUtils::CS_Fail
			);
			return;
		}

		if (Options.ExportMethod == GraphPrinter::EExportMethod::ImageFile)
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("SucceededOutput", "GraphEditor capture saved as"),
				GraphPrinter::FGraphPrinterUtils::CS_Success, 5.f,
				TArray<GraphPrinter::FNotificationInteraction>{
					GraphPrinter::FNotificationInteraction(
						FText::FromString(Filename),
						FSimpleDelegate::CreateLambda([Filename]()
						{
							GraphPrinter::FGraphPrinterUtils::OpenFolderWithExplorer(Filename);
						})
					)
				}
			);

#ifdef WITH_TEXT_CHUNK_HELPER
			// Embed node information in the output png image.
			// When copying to the clipboard, the process is skipped.
			if (Options.bIsIncludeNodeInfoInImageFile &&
				Options.ImageWriteOptions.Format == EDesiredImageFormat::PNG)
			{
				if (!WeakThis->WriteNodeInfoToTextChunk(GraphEditor, Filename, NodesToPrint, Options))
				{
					GraphPrinter::FGraphPrinterUtils::ShowNotification(
						LOCTEXT("FailedEmbedNodeInfoError", "Failed to write node information to png file."),
						GraphPrinter::FGraphPrinterUtils::CS_Fail
					);
				}
			}
#endif
		}
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		else if (Options.ExportMethod == GraphPrinter::EExportMethod::Clipboard)
		{
			if (WeakThis->CopyImageFileToClipboard(Filename, Options))
			{
				GraphPrinter::FGraphPrinterUtils::ShowNotification(
					LOCTEXT("SucceededClipboardCopy", "Succeeded to copy image to clipboard."),
					GraphPrinter::FGraphPrinterUtils::CS_Success
				);
			}
			else
			{
				GraphPrinter::FGraphPrinterUtils::ShowNotification(
					LOCTEXT("FailedClipboardCopy", "Failed to copy image to clipboard."),
					GraphPrinter::FGraphPrinterUtils::CS_Fail
				);
			}

			IFileManager::Get().Delete(*Filename, false, true);
		}
#endif
	};
	
	// Export the render target in the specified file format.
	ExportRenderTargetToImageFile(RenderTarget, Filename, Options);

	// As a symptomatic treatment for the problem that the first image output after startup is whitish,
	// the first output is re-output as many times as NumberOfRedrawsWhenFirstTime.
	if (IsFirstOutput.GetValue())
	{
		for (int32 Count = 0; Count < NumberOfReOutputWhenFirstTime; Count++)
		{
			FImageWriteOptions ImageWriteOptions = Options.ImageWriteOptions;
			ImageWriteOptions.bOverwriteFile = true;
			ImageWriteOptions.NativeOnComplete = nullptr;
			UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, ImageWriteOptions);
			IsFirstOutput.Switch();
		}	
	}
}

bool UGenericGraphPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	return GraphEditor.IsValid();
}

#ifdef WITH_TEXT_CHUNK_HELPER
void UGenericGraphPrinter::RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options)
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);

	// If don't have an active graph editor, end here.
	if (!GraphEditor.IsValid())
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotFoundGraphEditorError", "The graph editor isn't currently open."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	// Launch the file browser and select the png file.
	FString Filename;
	if (Options.HasValidSourceImageFilePath())
	{
		Filename = Options.GetSourceImageFilePath();
	}
	else
	{
		TArray<FString> Filenames;
		if (!GraphPrinter::FGraphPrinterUtils::OpenFileDialog(
			Filenames,
			TEXT("Select the png file that contains the node info"),
			UGraphPrinterSettings::Get().OutputDirectory.Path,
			TEXT(""),
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
	if (RestoreNodesFromTextChunk(GraphEditor, Filename, Options))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("SucceededRestore", "Restore nodes from"),
			GraphPrinter::FGraphPrinterUtils::CS_Success, 5.f,
			TArray<GraphPrinter::FNotificationInteraction>{
				GraphPrinter::FNotificationInteraction(
					FText::FromString(Filename),
					FSimpleDelegate::CreateLambda([Filename]()
					{
						GraphPrinter::FGraphPrinterUtils::OpenFolderWithExplorer(Filename);
					})
				)
			}
		);
	}
	else
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("FailedRestoreError", "Failed restore nodes."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
	}
}

bool UGenericGraphPrinter::CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	return GraphEditor.IsValid();
}
#endif

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPrinter;
}

TSharedPtr<SGraphEditorImpl> UGenericGraphPrinter::FindGraphEditor(const TSharedPtr<SWidget>& TargetWidget) const
{
	if (TargetWidget.IsValid())
	{
		return GraphPrinter::FGraphPrinterUtils::FindNearestChildGraphEditor(TargetWidget);
	}
	
	return GraphPrinter::FGraphPrinterUtils::GetActiveGraphEditor();
}

bool UGenericGraphPrinter::CalculateGraphDrawSizeAndViewLocation(
	FVector2D& DrawSize,
	FVector2D& ViewLocation,
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const GraphPrinter::FPrintWidgetOptions& Options
) const
{
#if BEFORE_UE_4_23
	// Special support is implemented because SGraphEditor::GetNumberOfSelectedNodes before UE4.23 always returns 0.
	const TSet<UObject*>& SelectedNodes = GraphEditor->GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
	{
		return false;
	}
#else
	if (GraphEditor->GetNumberOfSelectedNodes() == 0)
	{
		return false;
	}
#endif

	FSlateRect Bounds;
	if (!GraphEditor->GetBoundsForSelectedNodes(Bounds, Options.Padding))
	{
		return false;
	}
	DrawSize = Bounds.GetSize();
	ViewLocation = Bounds.GetTopLeft();

	return true;
}

void UGenericGraphPrinter::PrePrintGraphEditor(
	FVector2D& PreviousViewLocation, 
	float& PreviousZoomAmount,
	FGraphPanelSelectionSet& NodesToPrint,
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const FVector2D& DrawSize,
	const FVector2D& ViewLocation,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	// Set the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
	GraphEditor->GetViewLocation(PreviousViewLocation, PreviousZoomAmount);
	GraphEditor->SetViewLocation(ViewLocation, 1.f);

	NodesToPrint = GraphEditor->GetSelectedNodes();
	
	// Erase the drawing result so that the frame for which the node is selected does not appear.
	GraphEditor->ClearSelectionSet();
}

bool UGenericGraphPrinter::CanPrintGraphEditor(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const FVector2D& DrawSize,
	const FVector2D& ViewLocation,
	const GraphPrinter::FPrintWidgetOptions& Options
) const
{
	// Check draw size.
	if (Options.MaxImageSize > FVector2D::ZeroVector)
	{
		if (DrawSize.X > Options.MaxImageSize.X || DrawSize.Y > Options.MaxImageSize.Y)
		{
			return false;
		}
	}

	return true;
}

UTextureRenderTarget2D* UGenericGraphPrinter::DrawGraphToRenderTarget(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const FVector2D& DrawSize,
	const FVector2D& ViewLocation,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	// If there is a minimap, hide it only while drawing.
	const TSharedPtr<SWidget> Minimap = GraphPrinter::FGraphPrinterUtils::FindNearestChildMinimap(GraphEditor);
	TOptional<EVisibility> PreviousMinimapVisibility;
	if (Minimap.IsValid())
	{
		PreviousMinimapVisibility = Minimap->GetVisibility();
		Minimap->SetVisibility(EVisibility::Collapsed);
	}
	
	// If there is a title bar, hide it only while drawing.
	const TSharedPtr<SWidget> TitleBar = GraphEditor->GetTitleBar();
	TOptional<EVisibility> PreviousTitleBarVisibility;
	if (Options.bDrawOnlyGraph && TitleBar.IsValid())
	{
		PreviousTitleBarVisibility = TitleBar->GetVisibility();
		TitleBar->SetVisibility(EVisibility::Collapsed);
	}

	// Hide zoom magnification and graph type text while drawing.
	TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
	if (Options.bDrawOnlyGraph)
	{
		const TSharedPtr<SOverlay> Overlay = GraphPrinter::FGraphPrinterUtils::FindNearestChildOverlay(GraphEditor);
		TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = GraphPrinter::FGraphPrinterUtils::GetVisibleChildTextBlocks(Overlay);
		for (const TSharedPtr<STextBlock>& VisibleChildTextBlock : VisibleChildTextBlocks)
		{
			if (VisibleChildTextBlock.IsValid())
			{
				PreviousChildTextBlockVisibilities.Add(VisibleChildTextBlock, VisibleChildTextBlock->GetVisibility());
				VisibleChildTextBlock->SetVisibility(EVisibility::Collapsed);
			}
		}
	}
	
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(Options.bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Options.FilteringMode, Options.bUseGamma);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}
	if (Options.bUseGamma)
	{
		RenderTarget->bForceLinearGamma = true;
		RenderTarget->UpdateResourceImmediate(true);
	}

	// Since the drawing result may be corrupted the first time, draw multiple times.
	for (int32 Count = 0; Count < DrawTimes; Count++)
	{
		WidgetRenderer->DrawWidget(
			RenderTarget,
			GraphEditor.ToSharedRef(),
			Options.RenderingScale,
			DrawSize,
			0.f
		);
		FlushRenderingCommands();
	}

	BeginCleanup(WidgetRenderer);

	// Restores the visibility of the title bar,
	// zoom magnification text, and graph type text.
	if (Minimap.IsValid() && PreviousMinimapVisibility.IsSet())
	{
		Minimap->SetVisibility(PreviousMinimapVisibility.GetValue());
	}
	
	if (Options.bDrawOnlyGraph && TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
	{
		TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
	}

	if (Options.bDrawOnlyGraph)
	{
		for (const auto& PreviousChildTextBlockVisibility : PreviousChildTextBlockVisibilities)
		{
			TSharedPtr<STextBlock> TextBlock = PreviousChildTextBlockVisibility.Key;
			EVisibility PreviousVisibility = PreviousChildTextBlockVisibility.Value;
			if (TextBlock.IsValid())
			{
				TextBlock->SetVisibility(PreviousVisibility);
			}
		}
	}
	
	return RenderTarget;
}

void UGenericGraphPrinter::PostPrintGraphEditor(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const FVector2D& PreviousViewLocation,
	const float PreviousZoomAmount,
	const FGraphPanelSelectionSet& PreviousSelectedNodes,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	// Restore camera position and zoom magnification.
	GraphEditor->SetViewLocation(PreviousViewLocation, PreviousZoomAmount);

	// Restore the node selection status.
	GraphEditor->ClearSelectionSet();
	for (const auto& SelectedNode : PreviousSelectedNodes)
	{
		if (auto* GraphNode = Cast<UEdGraphNode>(SelectedNode))
		{
			GraphEditor->SetNodeSelection(GraphNode, true);
		}
	}
}

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
void UGenericGraphPrinter::PrepareCopyToClipboard(GraphPrinter::FPrintWidgetOptions& Options)
{
	// When copying to the clipboard, set the image format so that it can be copied.
	if (Options.ExportMethod == GraphPrinter::EExportMethod::Clipboard)
	{
		Options.ImageWriteOptions.Format = ClipboardImageExtension::FClipboardImageExtension::GetCopyableImageFormat();
	}
}
#endif

FString UGenericGraphPrinter::CreateFilename(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const GraphPrinter::FPrintWidgetOptions& Options
) const
{
	FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, GetGraphTitle(GraphEditor))
	);
	const FString& Extension = GraphPrinter::FGraphPrinterUtils::GetImageFileExtension(Options.ImageWriteOptions.Format);

	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(ValidatePathErrorText, GraphPrinter::FGraphPrinterUtils::CS_Fail);
		return FString();
	}

	// If the file cannot be overwritten, add a number after the file name.
	if (!Options.ImageWriteOptions.bOverwriteFile)
	{
		if (IFileManager::Get().FileExists(*FString(Filename + Extension)))
		{
			auto CombineFilenameAndIndex = [](const FString& Filename, int32 Index) -> FString
			{
				return FString::Printf(TEXT("%s_%d"), *Filename, Index);
			};

			int32 Index = 0;
			while (Index < TNumericLimits<int32>().Max())
			{
				const FString& FilenameWithExtension = CombineFilenameAndIndex(Filename, Index) + Extension;
				if (!IFileManager::Get().FileExists(*FilenameWithExtension))
				{
					break;
				}
				Index++;
			}
			Filename = CombineFilenameAndIndex(Filename, Index);
		}
	}

	return (Filename + Extension);
}

FString UGenericGraphPrinter::GetGraphTitle(const TSharedPtr<SGraphEditorImpl>& GraphEditor) const
{
	if (const UEdGraph* Graph = GraphEditor->GetCurrentGraph())
	{
		if (const UObject* Outer = Graph->GetOuter())
		{
			return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
		}
	}

	return TEXT("InvalidGraphEditor");
}

void UGenericGraphPrinter::ExportRenderTargetToImageFile(
	UTextureRenderTarget2D* RenderTarget,
	const FString& Filename,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, Options.ImageWriteOptions);
}

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
bool UGenericGraphPrinter::CopyImageFileToClipboard(
	const FString& Filename,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	return ClipboardImageExtension::FClipboardImageExtension::ClipboardCopy(Filename);
}
#endif

#ifdef WITH_TEXT_CHUNK_HELPER
bool UGenericGraphPrinter::WriteNodeInfoToTextChunk(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor,
	const FString& Filename,
	const FGraphPanelSelectionSet& NodesToPrint,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	// Make a shortcut key event for copy operation.
	FKeyEvent KeyEvent;
	if (!GraphPrinter::FGraphPrinterUtils::GetKeyEventFromUICommandInfo(FGenericCommands::Get().Copy, KeyEvent))
	{
		return false;
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(NodesToPrint, ExportedText);
	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ExportedText))
	{
		return false;
	}

	// Write data to png file using helper class.
	TMap<FString, FString> MapToWrite;
	MapToWrite.Add(PngTextChunkKey, ExportedText);

	const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(Filename);
	if (!TextChunk.IsValid())
	{
		return false;
	}
	return TextChunk->Write(MapToWrite);
}

bool UGenericGraphPrinter::RestoreNodesFromTextChunk(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor,
	const FString& Filename,
	const GraphPrinter::FRestoreWidgetOptions& Options
)
{
	// Read data from png file using helper class.
	TMap<FString, FString> MapToRead;
	const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(Filename);
	if (!TextChunk.IsValid())
	{
		return false;
	}
	if (!TextChunk->Read(MapToRead))
	{
		return false;
	}

	// Find information on valid nodes.
	if (!MapToRead.Contains(PngTextChunkKey))
	{
		return false;
	}
	FString TextToImport = MapToRead[PngTextChunkKey];

	// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
	int32 StartPosition = 0;
	const int32 TextLength = TextToImport.Len();
	const int32 HeaderLength = NodeInfoHeader.Len();
	for (int32 Index = 0; Index < TextLength - HeaderLength; Index++)
	{
		bool bIsMatch = true;
		for (int32 Offset = 0; Offset < HeaderLength; Offset++)
		{
			if (TextToImport[Index + Offset] != NodeInfoHeader[Offset])
			{
				bIsMatch = false;
			}
		}

		if (bIsMatch)
		{
			StartPosition = Index;
			break;
		}
	}
	if (StartPosition > 0)
	{
		TextToImport = TextToImport.Mid(StartPosition, TextLength - StartPosition);
	}

	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport))
	{
		return false;
	}

	TSet<UEdGraphNode*> UnuseImportedNodeSet;
	FEdGraphUtilities::ImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport, UnuseImportedNodeSet);

	return true;
}
#endif

GraphPrinter::FOneWayBool UGenericGraphPrinter::IsFirstOutput = true;

#ifdef WITH_TEXT_CHUNK_HELPER
const FString UGenericGraphPrinter::PngTextChunkKey = TEXT("GraphEditor");
const FString UGenericGraphPrinter::NodeInfoHeader = TEXT("Begin Object");
#endif

#undef LOCTEXT_NAMESPACE
