// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "GenericGraphPrinter/Utilities/GenericGraphPrinterUtils.h"
#include "GenericGraphPrinter/Types/PrintGraphOptions.h"
#include "SGraphEditorImpl.h"
#include "EdGraphUtilities.h"
#include "Widgets/Text/STextBlock.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

namespace GraphPrinter
{
#ifdef WITH_TEXT_CHUNK_HELPER
	namespace GenericGraphPrinter
	{
		namespace TextChunkDefine
		{
			// The key used when writing to a text chunk of a png file.
			static const FString PngTextChunkKey = TEXT("GraphEditor");

			// The beginning of the node information.
			static const FString NodeInfoHeader = TEXT("Begin Object");
		}
	}
#endif
	
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	template<class TPrintOptions, class TRestoreOptions>
	class TGraphPrinter
		: public TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>
	{
	public:
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintGraphOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintGraphOptions.");

		using Super = TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>;
		using TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>::PrintOptions;
		using TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>::RestoreOptions;
		using TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>::Widget;
		using TInnerWidgetPrinter<SGraphEditorImpl, TPrintOptions, TRestoreOptions>::WidgetPrinterParams;
		
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

		// IInnerWidgetPrinter interface.
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
			if (!IsValid(RestoreOptions))
			{
				return false;
			}
			
			const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(RestoreOptions->SearchTarget);
			return GraphEditor.IsValid();
		}
		// End of IInnerWidgetPrinter interface.

	protected:
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SGraphEditorImpl> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FindTargetWidgetFromSearchTarget(SearchTarget);
			}
	
			return FGenericGraphPrinterUtils::GetActiveGraphEditor();
		}
		virtual void PreCalculateDrawSize() override
		{
			// Holds the node selected by the user for printing and then restoring.
			GenericGraphPrinterParams.PreviousSelectedNodes = Widget->GetSelectedNodes();

			// Gets the range of the selected node and the position of the camera to use when drawing.
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
			// Sets the camera position to the upper left of the drawing range and set the zoom magnification to 1:1.
			Widget->GetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);
			Widget->SetViewLocation(GenericGraphPrinterParams.ViewLocation, 1.f);

			GenericGraphPrinterParams.NodesToPrint = Widget->GetSelectedNodes();
	
			// Erases the drawing result so that the frame for which the node is selected does not appear.
			Widget->ClearSelectionSet();

			// If there is a minimap, hides it only while drawing.
			GenericGraphPrinterParams.Minimap = FGenericGraphPrinterUtils::FindNearestChildMinimap(Widget);
			if (GenericGraphPrinterParams.Minimap.IsValid())
			{
				GenericGraphPrinterParams.PreviousMinimapVisibility = GenericGraphPrinterParams.Minimap->GetVisibility();
				GenericGraphPrinterParams.Minimap->SetVisibility(EVisibility::Collapsed);
			}
	
			// If there is a title bar, hides it only while drawing.
			GenericGraphPrinterParams.TitleBar = Widget->GetTitleBar();
			if (PrintOptions->bDrawOnlyGraph && GenericGraphPrinterParams.TitleBar.IsValid())
			{
				GenericGraphPrinterParams.PreviousTitleBarVisibility = GenericGraphPrinterParams.TitleBar->GetVisibility();
				GenericGraphPrinterParams.TitleBar->SetVisibility(EVisibility::Collapsed);
			}

			// Hides zoom magnification and graph type text while drawing.
			if (PrintOptions->bDrawOnlyGraph)
			{
				const TSharedPtr<SOverlay> Overlay = FWidgetPrinterUtils::FindNearestChildOverlay(Widget);
				TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = FGenericGraphPrinterUtils::GetVisibleChildTextBlocks(Overlay);
				for (const TSharedPtr<STextBlock>& VisibleChildTextBlock : VisibleChildTextBlocks)
				{
					if (VisibleChildTextBlock.IsValid())
					{
						GenericGraphPrinterParams.PreviousChildTextBlockVisibilities.Add(VisibleChildTextBlock, VisibleChildTextBlock->GetVisibility());
						VisibleChildTextBlock->SetVisibility(EVisibility::Collapsed);
					}
				}
			}
		}
		virtual void PostDrawWidget() override
		{
			// Restores the visibility of the title bar, zoom magnification text, and graph type text.
			if (GenericGraphPrinterParams.Minimap.IsValid() && GenericGraphPrinterParams.PreviousMinimapVisibility.IsSet())
			{
				GenericGraphPrinterParams.Minimap->SetVisibility(GenericGraphPrinterParams.PreviousMinimapVisibility.GetValue());
			}
	
			if (PrintOptions->bDrawOnlyGraph && GenericGraphPrinterParams.TitleBar.IsValid() && GenericGraphPrinterParams.PreviousTitleBarVisibility.IsSet())
			{
				GenericGraphPrinterParams.TitleBar->SetVisibility(GenericGraphPrinterParams.PreviousTitleBarVisibility.GetValue());
			}

			if (PrintOptions->bDrawOnlyGraph)
			{
				for (const auto& PreviousChildTextBlockVisibility : GenericGraphPrinterParams.PreviousChildTextBlockVisibilities)
				{
					TSharedPtr<STextBlock> TextBlock = PreviousChildTextBlockVisibility.Key;
					EVisibility PreviousVisibility = PreviousChildTextBlockVisibility.Value;
					if (TextBlock.IsValid())
					{
						TextBlock->SetVisibility(PreviousVisibility);
					}
				}
			}
			
			// Restores camera position and zoom magnification.
			Widget->SetViewLocation(GenericGraphPrinterParams.PreviousViewLocation, GenericGraphPrinterParams.PreviousZoomAmount);

			// Restores the node selection status.
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
			FString Title;
			GetGraphTitle(Widget, Title);
			return Title;
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

			// Writes data to png file using helper class.
			TMap<FString, FString> MapToWrite;
			MapToWrite.Add(GenericGraphPrinter::TextChunkDefine::PngTextChunkKey, ExportedText);

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
			// Reads data from png file using helper class.
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

			// Finds information on valid nodes.
			if (!MapToRead.Contains(GenericGraphPrinter::TextChunkDefine::PngTextChunkKey))
			{
				return false;
			}
			FString TextToImport = MapToRead[GenericGraphPrinter::TextChunkDefine::PngTextChunkKey];

			// Unnecessary characters may be mixed in at the beginning of the text, so inspects and corrects it.
			FGraphPrinterUtils::ClearUnnecessaryCharactersFromHead(TextToImport, GenericGraphPrinter::TextChunkDefine::NodeInfoHeader);

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
		// End of TInnerWidgetPrinter interface.

	public:
		// Finds the target widget from the search target.
		static TSharedPtr<SGraphEditorImpl> FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget)
		{
			const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(SearchTarget);
			return FGenericGraphPrinterUtils::FindNearestChildGraphEditor(DockingTabStack);
		}
		
		// Returns the title from the graph in the format "[asset name]-[graph title]".
		static bool GetGraphTitle(const TSharedPtr<SGraphEditorImpl>& GraphEditor, FString& Title)
		{
			Title = TEXT("InvalidGraphEditor");
			
			if (!GraphEditor.IsValid())
			{
				return false;
			}

			const UEdGraph* Graph = GraphEditor->GetCurrentGraph();
			if (!IsValid(Graph))
			{
				return false;
			}

			const UObject* Outer = Graph->GetOuter();
			if (!IsValid(Outer))
			{
				return false;
			}

			Title = FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			return true;
		}
		
	protected:
		// Calculates the range and view location to use when drawing the graph editor.
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

			// The minimap widget for graph editors added by the GraphMinimap plugin.
			TSharedPtr<SWidget> Minimap;

			// The original visibility of the minimap in the graph editor.
			TOptional<EVisibility> PreviousMinimapVisibility;

			// The title bar widget that appears at the top of the graph editor.
			TSharedPtr<SWidget> TitleBar;

			// The original visibility of title bar in graph editor
			TOptional<EVisibility> PreviousTitleBarVisibility;

			// The original visibility of text in graph editor overlays.
			TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
		};
		FGenericGraphPrinterParams GenericGraphPrinterParams;
	};

	class GENERICGRAPHPRINTER_API FGenericGraphPrinter : public TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>;
		
	public:
		// Constructors.
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
