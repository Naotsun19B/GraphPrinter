// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/WidgetPrinters/GenericGraphPrinter.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "SGraphEditorImpl.h"
#include "EdGraphUtilities.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

int32 UGenericGraphPrinter::GetPriority() const
{
	return GenericGraphPrinterPrinter;
}

UPrintWidgetOptions* UGenericGraphPrinter::CreateDefaultPrintOptions() const
{
	auto* PrintOptions = NewObject<UPrintGraphOptions>();
	return PrintOptions;
}

TSharedPtr<SWidget> UGenericGraphPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
{
	if (SearchTarget.IsValid())
	{
		return GraphPrinter::FWidgetPrinterUtils::FindNearestChildGraphEditor(SearchTarget);
	}
	
	return GraphPrinter::FWidgetPrinterUtils::GetActiveGraphEditor();
}

void UGenericGraphPrinter::PreCalculateDrawSize()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();

	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		// Holds the node selected by the user for printing and then restoring.
		GenericGraphPrinterParams.PreviousSelectedNodes = GraphEditor->GetSelectedNodes();

		// Get the range of the selected node and the position of the camera to use when drawing.
		if (PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::All)
		{
			GraphEditor->SelectAllNodes();
		}
	}
}

bool UGenericGraphPrinter::CalculateDrawSize(FVector2D& DrawSize)
{
	return CalculateGraphDrawSizeAndViewLocation(DrawSize, GenericGraphPrinterParams.ViewLocation);
}

void UGenericGraphPrinter::PreDrawWidget()
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		// Set the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
		GraphEditor->GetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);
		GraphEditor->SetViewLocation(GenericGraphPrinterParams.ViewLocation, 1.f);

		GenericGraphPrinterParams.NodesToPrint = GraphEditor->GetSelectedNodes();
	
		// Erase the drawing result so that the frame for which the node is selected does not appear.
		GraphEditor->ClearSelectionSet();
	}
}

UTextureRenderTarget2D* UGenericGraphPrinter::DrawWidgetToRenderTarget()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	// If there is a minimap, hide it only while drawing.
	const TSharedPtr<SWidget> Minimap = GraphPrinter::FWidgetPrinterUtils::FindNearestChildMinimap(Widget);
	TOptional<EVisibility> PreviousMinimapVisibility;
	if (Minimap.IsValid())
	{
		PreviousMinimapVisibility = Minimap->GetVisibility();
		Minimap->SetVisibility(EVisibility::Collapsed);
	}

	// If there is a title bar, hide it only while drawing.
	TSharedPtr<SWidget> TitleBar = nullptr;
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		TitleBar = GraphEditor->GetTitleBar();
	}

	bool bDrawOnlyGraph = false;
	if (const auto* PrintGraphOptions = Cast<UPrintGraphOptions>(PrintOptions))
	{
		bDrawOnlyGraph = PrintGraphOptions->bDrawOnlyGraph;
	}
	
	TOptional<EVisibility> PreviousTitleBarVisibility;
	if (bDrawOnlyGraph && TitleBar.IsValid())
	{
		PreviousTitleBarVisibility = TitleBar->GetVisibility();
		TitleBar->SetVisibility(EVisibility::Collapsed);
	}

	// Hide zoom magnification and graph type text while drawing.
	TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
	if (bDrawOnlyGraph)
	{
		const TSharedPtr<SOverlay> Overlay = GraphPrinter::FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
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

	UTextureRenderTarget2D* RenderTarget = Super::DrawWidgetToRenderTarget();

	// Restores the visibility of the title bar,
	// zoom magnification text, and graph type text.
	if (Minimap.IsValid() && PreviousMinimapVisibility.IsSet())
	{
		Minimap->SetVisibility(PreviousMinimapVisibility.GetValue());
	}

	if (bDrawOnlyGraph && TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
	{
		TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
	}

	if (bDrawOnlyGraph)
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

void UGenericGraphPrinter::PostDrawWidget()
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		// Restore camera position and zoom magnification.
		GraphEditor->SetViewLocation(
			GenericGraphPrinterParams.PreviousViewLocation,
			GenericGraphPrinterParams.PreviousZoomAmount
		);

		// Restore the node selection status.
		GraphEditor->ClearSelectionSet();
		for (const auto& SelectedNode : GenericGraphPrinterParams.PreviousSelectedNodes)
		{
			if (auto* GraphNode = Cast<UEdGraphNode>(SelectedNode))
			{
				GraphEditor->SetNodeSelection(GraphNode, true);
			}
		}
	}
}

FString UGenericGraphPrinter::GetWidgetTitle() const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
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

bool UGenericGraphPrinter::WriteWidgetInfoToTextChunk()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (!GraphEditor.IsValid())
	{
		return false;
	}
	
	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(GenericGraphPrinterParams.NodesToPrint, ExportedText);
	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ExportedText))
	{
		return false;
	}

	// Write data to png file using helper class.
	TMap<FString, FString> MapToWrite;
	MapToWrite.Add(PngTextChunkKey, ExportedText);

	const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(WidgetPrinterParams.Filename);
	if (!TextChunk.IsValid())
	{
		return false;
	}
	return TextChunk->Write(MapToWrite);
#else
	return false;
#endif
}

bool UGenericGraphPrinter::RestoreWidgetFromTextChunk()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (!GraphEditor.IsValid())
	{
		return false;
	}
	
	// Read data from png file using helper class.
	TMap<FString, FString> MapToRead;
	const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(WidgetPrinterParams.Filename);
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

bool UGenericGraphPrinter::ShouldAlwaysPrintAll() const
{
	return false;
}

bool UGenericGraphPrinter::CalculateGraphDrawSizeAndViewLocation(FVector2D& DrawSize, FVector2D& ViewLocation)
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (!GraphEditor.IsValid())
	{
		return false;
	}
	
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
	if (!GraphEditor->GetBoundsForSelectedNodes(Bounds, PrintOptions->Padding))
	{
		return false;
	}
	DrawSize = Bounds.GetSize();
	ViewLocation = Bounds.GetTopLeft();

	return true;
}

#ifdef WITH_TEXT_CHUNK_HELPER
const FString UGenericGraphPrinter::PngTextChunkKey = TEXT("GraphEditor");
const FString UGenericGraphPrinter::NodeInfoHeader = TEXT("Begin Object");
#endif
