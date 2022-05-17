// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "GraphPrinter/Utilities/GraphPrinterSettings.h"
#include "GraphPrinter/Utilities/WidgetPrinterUtils.h"
#include "GraphPrinter/Utilities/CastSlateWidget.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "SGraphEditorImpl.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "EdGraphUtilities.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

#define LOCTEXT_NAMESPACE "GenericGraphPrinter"

FString UGenericGraphPrinter::GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const
{
	const TSharedPtr<SGraphEditorImpl>& GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		if (const UEdGraph* Graph = GraphEditor->GetCurrentGraph())
		{
			if (const UObject* Outer = Graph->GetOuter())
			{
				return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			}
		}
	}

	return TEXT("InvalidGraphEditor");
}

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

	// If the restore function cannot be used, the node information will not be embedded.
	{
		GraphPrinter::FRestoreWidgetOptions RestoreWidgetOptions;
		RestoreWidgetOptions.TargetWidget = Options.TargetWidget;
		if (!CanRestoreWidget(RestoreWidgetOptions))
		{
			Options.bIsIncludeNodeInfoInImageFile = false;
		}
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

	const bool bIsPrintableSize = IsPrintableSize(GraphEditor, DrawSize, Options);

	// Draw the graph editor on the render target.
	UTextureRenderTarget2D* RenderTarget = nullptr;
	if (bIsPrintableSize)
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

	if (!bIsPrintableSize)
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
}

bool UGenericGraphPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	return GraphEditor.IsValid();
}

void UGenericGraphPrinter::RestoreWidget(GraphPrinter::FRestoreWidgetOptions Options)
{
#ifdef WITH_TEXT_CHUNK_HELPER
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
#endif
}

bool UGenericGraphPrinter::CanRestoreWidget(const GraphPrinter::FRestoreWidgetOptions& Options) const
{
#ifdef WITH_TEXT_CHUNK_HELPER
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	return GraphEditor.IsValid();
#else
	return false;
#endif
}

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPrinter;
}

TSharedPtr<SGraphEditorImpl> UGenericGraphPrinter::FindGraphEditor(const TSharedPtr<SWidget>& TargetWidget) const
{
	if (TargetWidget.IsValid())
	{
		return GraphPrinter::FWidgetPrinterUtils::FindNearestChildGraphEditor(TargetWidget);
	}
	
	return GraphPrinter::FWidgetPrinterUtils::GetActiveGraphEditor();
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

UTextureRenderTarget2D* UGenericGraphPrinter::DrawGraphToRenderTarget(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor, 
	const FVector2D& DrawSize,
	const FVector2D& ViewLocation,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	// If there is a minimap, hide it only while drawing.
	const TSharedPtr<SWidget> Minimap = GraphPrinter::FWidgetPrinterUtils::FindNearestChildMinimap(GraphEditor);
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
		const TSharedPtr<SOverlay> Overlay = GraphPrinter::FWidgetPrinterUtils::FindNearestChildOverlay(GraphEditor);
		TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = GraphPrinter::FWidgetPrinterUtils::GetVisibleChildTextBlocks(Overlay);
		for (const TSharedPtr<STextBlock>& VisibleChildTextBlock : VisibleChildTextBlocks)
		{
			if (VisibleChildTextBlock.IsValid())
			{
				PreviousChildTextBlockVisibilities.Add(VisibleChildTextBlock, VisibleChildTextBlock->GetVisibility());
				VisibleChildTextBlock->SetVisibility(EVisibility::Collapsed);
			}
		}
	}
	
	UTextureRenderTarget2D* RenderTarget = DrawWidgetToRenderTarget(
		GraphEditor,
		DrawSize,
		Options
	);

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

bool UGenericGraphPrinter::WriteNodeInfoToTextChunk(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor,
	const FString& Filename,
	const FGraphPanelSelectionSet& NodesToPrint,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
#ifdef WITH_TEXT_CHUNK_HELPER
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
#else
	return false;
#endif
}

bool UGenericGraphPrinter::RestoreNodesFromTextChunk(
	const TSharedPtr<SGraphEditorImpl>& GraphEditor,
	const FString& Filename,
	const GraphPrinter::FRestoreWidgetOptions& Options
)
{
#ifdef WITH_TEXT_CHUNK_HELPER
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
#else
	return false;
#endif
}

#ifdef WITH_TEXT_CHUNK_HELPER
const FString UGenericGraphPrinter::PngTextChunkKey = TEXT("GraphEditor");
const FString UGenericGraphPrinter::NodeInfoHeader = TEXT("Begin Object");
#endif

#undef LOCTEXT_NAMESPACE
