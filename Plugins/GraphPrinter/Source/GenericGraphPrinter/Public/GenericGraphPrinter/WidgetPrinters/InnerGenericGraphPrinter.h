// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerPrinter.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "SGraphEditorImpl.h"
#include "EdGraphUtilities.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class GENERICGRAPHPRINTER_API FGenericGraphPrinter
		: public TInnerPrinter<SGraphEditorImpl, UPrintGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerPrinter<SGraphEditorImpl, UPrintGraphOptions, URestoreWidgetOptions>;
		
	public:
		// Constructor.
		FGenericGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
			, GenericGraphPrinterParams()
		{
		}
		FGenericGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
			, GenericGraphPrinterParams()
		{
		}

		// TInnerPrinter interface.
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget())
			{
				if (PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::Selected)
				{
					const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
					if (GraphEditor.IsValid())
					{
						const TSet<UObject*>& SelectedNodes = GraphEditor->GetSelectedNodes();
						if (SelectedNodes.Num() == 0)
						{
							return false;
						}
					}
				}
				
				return true;
			}

			return false;
		}
		virtual bool CanRestoreWidget() const override
		{
			if (!IsValid(PrintOptions))
			{
				return false;
			}
			
			if (ShouldAlwaysPrintAll() && PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::Selected)
			{
				return false;
			}

			const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
			return GraphEditor.IsValid();
		}
		virtual TSharedPtr<SGraphEditorImpl> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FWidgetPrinterUtils::FindNearestChildGraphEditor(SearchTarget);
			}
	
			return FWidgetPrinterUtils::GetActiveGraphEditor();
		}
		virtual void PreCalculateDrawSize() override
		{
			// Holds the node selected by the user for printing and then restoring.
			GenericGraphPrinterParams.PreviousSelectedNodes = Widget->GetSelectedNodes();

			// Get the range of the selected node and the position of the camera to use when drawing.
			if (PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::All)
			{
				Widget->SelectAllNodes();
			}
		}
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override
		{
			return CalculateGraphDrawSizeAndViewLocation(DrawSize, GenericGraphPrinterParams.ViewLocation);
		}
		virtual void PreDrawWidget() override
		{
			// Set the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
			Widget->GetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);
			Widget->SetViewLocation(GenericGraphPrinterParams.ViewLocation, 1.f);

			GenericGraphPrinterParams.NodesToPrint = Widget->GetSelectedNodes();
	
			// Erase the drawing result so that the frame for which the node is selected does not appear.
			Widget->ClearSelectionSet();
		}
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override
		{
			// If there is a minimap, hide it only while drawing.
			const TSharedPtr<SWidget> Minimap = FWidgetPrinterUtils::FindNearestChildMinimap(Widget);
			TOptional<EVisibility> PreviousMinimapVisibility;
			if (Minimap.IsValid())
			{
				PreviousMinimapVisibility = Minimap->GetVisibility();
				Minimap->SetVisibility(EVisibility::Collapsed);
			}
	
			// If there is a title bar, hide it only while drawing.
			const TSharedPtr<SWidget> TitleBar = Widget->GetTitleBar();
			TOptional<EVisibility> PreviousTitleBarVisibility;
			if (PrintOptions->bDrawOnlyGraph && TitleBar.IsValid())
			{
				PreviousTitleBarVisibility = TitleBar->GetVisibility();
				TitleBar->SetVisibility(EVisibility::Collapsed);
			}

			// Hide zoom magnification and graph type text while drawing.
			TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
			if (PrintOptions->bDrawOnlyGraph)
			{
				const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
				TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = FWidgetPrinterUtils::GetVisibleChildTextBlocks(Overlay);
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
	
			if (PrintOptions->bDrawOnlyGraph && TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
			{
				TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
			}

			if (PrintOptions->bDrawOnlyGraph)
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
		virtual void PostDrawWidget() override
		{
			// Restore camera position and zoom magnification.
			Widget->SetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);

			// Restore the node selection status.
			Widget->ClearSelectionSet();
			for (const auto& SelectedNode : GenericGraphPrinterParams.PreviousSelectedNodes)
			{
				if (auto* GraphNode = Cast<UEdGraphNode>(SelectedNode))
				{
					Widget->SetNodeSelection(GraphNode, true);
				}
			}
		}
		virtual FString GetWidgetTitle() override
		{
			if (const UEdGraph* Graph = Widget->GetCurrentGraph())
			{
				if (const UObject* Outer = Graph->GetOuter())
				{
					return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
				}
			}
			
			return TEXT("InvalidGraphEditor");
		}
		virtual bool WriteWidgetInfoToTextChunk() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
			FString ExportedText;
			FEdGraphUtilities::ExportNodesToText(GenericGraphPrinterParams.NodesToPrint, ExportedText);
			if (!FEdGraphUtilities::CanImportNodesFromText(Widget->GetCurrentGraph(), ExportedText))
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
		virtual bool RestoreWidgetFromTextChunk() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
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

			if (!FEdGraphUtilities::CanImportNodesFromText(Widget->GetCurrentGraph(), TextToImport))
			{
				return false;
			}

			TSet<UEdGraphNode*> UnuseImportedNodeSet;
			FEdGraphUtilities::ImportNodesFromText(Widget->GetCurrentGraph(), TextToImport, UnuseImportedNodeSet);

			return true;
#else
			return false;
#endif
		}
		virtual bool ShouldAlwaysPrintAll() const override
		{
			return false;
		}
		// End of TInnerPrinter interface.

	protected:
		// Calculate the range and view location to use when drawing the graph editor.
		virtual bool CalculateGraphDrawSizeAndViewLocation(FVector2D& DrawSize, FVector2D& ViewLocation)
		{
			const TSet<UObject*>& SelectedNodes = Widget->GetSelectedNodes();
			if (SelectedNodes.Num() == 0)
			{
				return false;
			}

			FSlateRect Bounds;
			if (!Widget->GetBoundsForSelectedNodes(Bounds, PrintOptions->Padding))
			{
				return false;
			}
			DrawSize = Bounds.GetSize();
			ViewLocation = Bounds.GetTopLeft();

			return true;
		}
		
	protected:
		// A group of parameters that must be retained for processing.
		struct FGenericGraphPrinterParams
		{
			// In order to operate the camera position and zoom magnification for drawing, keep the previous state and restore it.
			FGraphPanelSelectionSet PreviousSelectedNodes;
			FVector2D PreviousViewLocation;
			float PreviousZoomAmount;

			// The nodes to be drawn.
			FGraphPanelSelectionSet NodesToPrint;
			
			// The size of the image to output.
			FVector2D ViewLocation;
		};
		FGenericGraphPrinterParams GenericGraphPrinterParams;

#ifdef WITH_TEXT_CHUNK_HELPER
	protected:
		// Key used when writing to a text chunk of a png file.
		static const FString PngTextChunkKey;

		// The beginning of the node information.
		static const FString NodeInfoHeader;
#endif
	};
}