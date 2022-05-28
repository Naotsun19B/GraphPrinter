// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterUtils.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "SGraphEditorImpl.h"
#include "EdGraphUtilities.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

namespace GraphPrinter
{
#ifdef WITH_TEXT_CHUNK_HELPER
	namespace TextChunkDefine
	{
		// Key used when writing to a text chunk of a png file.
		static const FString PngTextChunkKey = TEXT("GraphEditor");

		// The beginning of the node information.
		static const FString NodeInfoHeader = TEXT("Begin Object");
	}
#endif
	
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	template<class TPrintGraphOptions, class TRestoreOptions>
	class TGraphPrinter
		: public TInnerWidgetPrinter<SGraphEditorImpl, TPrintGraphOptions, TRestoreOptions>
	{
	public:
		static_assert(TIsDerivedFrom<TPrintGraphOptions, UPrintGraphOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintGraphOptions.");
		
		using Super = TInnerWidgetPrinter<SGraphEditorImpl, TPrintGraphOptions, TRestoreOptions>;
		
	public:
		// Constructor.
		TGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
			, GenericGraphPrinterParams()
		{
		}
		TGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
			, GenericGraphPrinterParams()
		{
		}

		// TInnerWidgetPrinter interface.
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget())
			{
				if (this->PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::Selected)
				{
					const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(this->PrintOptions->SearchTarget);
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
			if (!IsValid(this->RestoreOptions))
			{
				return false;
			}
			
			const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(this->RestoreOptions->SearchTarget);
			return GraphEditor.IsValid();
		}
		virtual TSharedPtr<SGraphEditorImpl> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FGenericGraphPrinterUtils::FindNearestChildGraphEditor(SearchTarget);
			}
	
			return FGenericGraphPrinterUtils::GetActiveGraphEditor();
		}
		virtual void PreCalculateDrawSize() override
		{
			// Holds the node selected by the user for printing and then restoring.
			GenericGraphPrinterParams.PreviousSelectedNodes = this->Widget->GetSelectedNodes();

			// Get the range of the selected node and the position of the camera to use when drawing.
			if (this->PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::All)
			{
				this->Widget->SelectAllNodes();
			}
		}
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override
		{
			return CalculateGraphDrawSizeAndViewLocation(DrawSize, GenericGraphPrinterParams.ViewLocation);
		}
		virtual void PreDrawWidget() override
		{
			// Set the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
			this->Widget->GetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);
			this->Widget->SetViewLocation(GenericGraphPrinterParams.ViewLocation, 1.f);

			GenericGraphPrinterParams.NodesToPrint = this->Widget->GetSelectedNodes();
	
			// Erase the drawing result so that the frame for which the node is selected does not appear.
			this->Widget->ClearSelectionSet();
		}
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override
		{
			// If there is a minimap, hide it only while drawing.
			const TSharedPtr<SWidget> Minimap = FGenericGraphPrinterUtils::FindNearestChildMinimap(this->Widget);
			TOptional<EVisibility> PreviousMinimapVisibility;
			if (Minimap.IsValid())
			{
				PreviousMinimapVisibility = Minimap->GetVisibility();
				Minimap->SetVisibility(EVisibility::Collapsed);
			}
	
			// If there is a title bar, hide it only while drawing.
			const TSharedPtr<SWidget> TitleBar = this->Widget->GetTitleBar();
			TOptional<EVisibility> PreviousTitleBarVisibility;
			if (this->PrintOptions->bDrawOnlyGraph && TitleBar.IsValid())
			{
				PreviousTitleBarVisibility = TitleBar->GetVisibility();
				TitleBar->SetVisibility(EVisibility::Collapsed);
			}

			// Hide zoom magnification and graph type text while drawing.
			TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
			if (this->PrintOptions->bDrawOnlyGraph)
			{
				const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(this->Widget);
				TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = FGenericGraphPrinterUtils::GetVisibleChildTextBlocks(Overlay);
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
	
			if (this->PrintOptions->bDrawOnlyGraph && TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
			{
				TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
			}

			if (this->PrintOptions->bDrawOnlyGraph)
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
			this->Widget->SetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);

			// Restore the node selection status.
			this->Widget->ClearSelectionSet();
			for (const auto& SelectedNode : GenericGraphPrinterParams.PreviousSelectedNodes)
			{
				if (auto* GraphNode = Cast<UEdGraphNode>(SelectedNode))
				{
					this->Widget->SetNodeSelection(GraphNode, true);
				}
			}
		}
		virtual FString GetWidgetTitle() override
		{
			if (const UEdGraph* Graph = this->Widget->GetCurrentGraph())
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
			if (!FEdGraphUtilities::CanImportNodesFromText(this->Widget->GetCurrentGraph(), ExportedText))
			{
				return false;
			}

			// Write data to png file using helper class.
			TMap<FString, FString> MapToWrite;
			MapToWrite.Add(TextChunkDefine::PngTextChunkKey, ExportedText);

			const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(this->WidgetPrinterParams.Filename);
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
			const TSharedPtr<TextChunkHelper::ITextChunk> TextChunk = TextChunkHelper::ITextChunkHelper::Get().CreateTextChunk(this->WidgetPrinterParams.Filename);
			if (!TextChunk.IsValid())
			{
				return false;
			}
			if (!TextChunk->Read(MapToRead))
			{
				return false;
			}

			// Find information on valid nodes.
			if (!MapToRead.Contains(TextChunkDefine::PngTextChunkKey))
			{
				return false;
			}
			FString TextToImport = MapToRead[TextChunkDefine::PngTextChunkKey];

			// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
			int32 StartPosition = 0;
			const int32 TextLength = TextToImport.Len();
			const int32 HeaderLength = TextChunkDefine::NodeInfoHeader.Len();
			for (int32 Index = 0; Index < TextLength - HeaderLength; Index++)
			{
				bool bIsMatch = true;
				for (int32 Offset = 0; Offset < HeaderLength; Offset++)
				{
					if (TextToImport[Index + Offset] != TextChunkDefine::NodeInfoHeader[Offset])
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

			if (!FEdGraphUtilities::CanImportNodesFromText(this->Widget->GetCurrentGraph(), TextToImport))
			{
				return false;
			}

			TSet<UEdGraphNode*> UnuseImportedNodeSet;
			FEdGraphUtilities::ImportNodesFromText(this->Widget->GetCurrentGraph(), TextToImport, UnuseImportedNodeSet);

			return true;
#else
			return false;
#endif
		}
		virtual bool ShouldAlwaysPrintAll() const override
		{
			return false;
		}
		// End of TInnerWidgetPrinter interface.

	protected:
		// Calculate the range and view location to use when drawing the graph editor.
		virtual bool CalculateGraphDrawSizeAndViewLocation(FVector2D& DrawSize, FVector2D& ViewLocation)
		{
			const TSet<UObject*>& SelectedNodes = this->Widget->GetSelectedNodes();
			if (SelectedNodes.Num() == 0)
			{
				return false;
			}

			FSlateRect Bounds;
			if (!this->Widget->GetBoundsForSelectedNodes(Bounds, this->PrintOptions->Padding))
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
	};

	class GENERICGRAPHPRINTER_API FGenericGraphPrinter : public TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>;
		
	public:
		FGenericGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FGenericGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}
	};
}
