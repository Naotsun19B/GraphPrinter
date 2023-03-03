// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "SDetailsView.h"
#include "SActorDetails.h"
#include "EdGraphUtilities.h"

namespace GraphPrinter
{
	/**
	 * I want to hide the implementation to the cpp file side in order to forcibly access the protected property.
	 */
	class FDetailsViewBaseAccessor
	{
	protected:
		// I want to hide the implementation to the cpp file side in order to forcibly access the protected property.
		// Therefore, it is implemented in a specialized child class.
		static TSharedPtr<SDetailTree> ExtractDetailTree(TSharedRef<SDetailsViewBase> DetailsViewBase);
		static FDetailNodeList& ExtractRootTreeNodes(TSharedRef<SDetailsViewBase> DetailsViewBase);
	};
	
	/**
	 * An inner class with the ability to print and restore details panel.
	 */
	template<class TWidget, class TPrintOptions, class TRestoreOptions>
	class TDetailsPanelPrinter
		: public TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>
		, public FDetailsViewBaseAccessor
	{
	public:
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintDetailsPanelOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintDetailsPanelOptions.");
		
		using Super = TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::PrintOptions;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::RestoreOptions;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::Widget;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::WidgetPrinterParams;
		
	public:
		// Constructor.
		TDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
			, DetailsPanelPrinterParams()
		{
		}
		TDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
			, DetailsPanelPrinterParams()
		{
		}

		// IInnerWidgetPrinter interface.
		virtual void PrintWidget() override
		{
			if (!IsValid(PrintOptions))
			{
				return;
			}
			
			DetailsPanelPrinterParams.DetailsView = FindDetailsView(FindTargetWidget(PrintOptions->SearchTarget));
			if (DetailsPanelPrinterParams.DetailsView.IsValid())
			{
				Super::PrintWidget();
			}
		}
		virtual void RestoreWidget() override
		{
			if (!IsValid(RestoreOptions))
			{
				return;
			}
			
			DetailsPanelPrinterParams.DetailsView = FindDetailsView(FindTargetWidget(RestoreOptions->SearchTarget));
			if (DetailsPanelPrinterParams.DetailsView.IsValid())
			{
				Super::RestoreWidget();
			}
		}
		virtual bool CanRestoreWidget() const override
		{
			if (IsValid(RestoreOptions))
			{
				const TSharedPtr<SDetailsView> DetailsView = FindDetailsView(FindTargetWidget(RestoreOptions->SearchTarget));
				if (DetailsView.IsValid())
				{
					return DetailsView->IsPropertyEditingEnabled();
				}
			}

			return false;
		}
		// End of IInnerWidgetPrinter interface.
		
		// TInnerWidgetPrinter interface.
		virtual void PreCalculateDrawSize() override
		{
			DetailsPanelPrinterParams.PreCalculateDetailsViewWidth = Widget->GetDesiredSize().X;
			
			const TSharedRef<SDetailTree> DetailsTree = GetDetailTree();

			// Cache the expanded state of each item if necessary.
			if (PrintOptions->bIsIncludeExpansionStateInImageFile)
			{
				TFunction<void(FDetailNodeList&, const FString&)> CacheExpansionStateRecursive =
					[&](FDetailNodeList& Nodes, const FString& ParentNodeName)
					{
						for (const auto& Node : Nodes)
						{
							const FString NodeName = Node->GetNodeName().ToString();
							FString CombinedNodeName = NodeName;
							if (!ParentNodeName.IsEmpty())
							{
								CombinedNodeName = FString::Join(TArray<FString>{ CombinedNodeName, NodeName }, TEXT("-"));
							}
							
							DetailsPanelPrinterParams.ExpansionStateMap.Add(CombinedNodeName, DetailsTree->IsItemExpanded(Node));

							FDetailNodeList Children;
							Node->GetChildren(Children);
							CacheExpansionStateRecursive(Children, NodeName);
						}
					};

				FDetailNodeList& RootTreeNodes = GetRootTreeNodes();
				CacheExpansionStateRecursive(RootTreeNodes, TEXT(""));
			}
			
			// The current scroll bar position is cached and scrolled to the bottom
			// because the correct drawing result cannot be obtained unless the scroll is scrolled to the bottom.
			DetailsPanelPrinterParams.ScrollOffset = DetailsTree->GetScrollOffset();
			DetailsTree->SetScrollOffset(DetailsTree->GetScrollDistance().Y + 100.f);

#if UE_5_00_OR_LATER
			DetailsPanelPrinterParams.DetailsView->MarkPrepassAsDirty();
#else
			DetailsPanelPrinterParams.DetailsView->InvalidatePrepass();
#endif
			DetailsPanelPrinterParams.DetailsView->SlatePrepass();
		}
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override
		{
			DrawSize = Widget->GetDesiredSize();
			
			// When drawing a large details view, the width becomes smaller according to the height, so the width before scrolling is used.
			DrawSize.X = DetailsPanelPrinterParams.PreCalculateDetailsViewWidth;
			
			// Increase the height by the amount of padding in the editor preferences.
			WidgetPrinterParams.DrawSize.Y += PrintOptions->Padding;
			
			return true;
		}
		virtual void PostDrawWidget() override
		{
			// Restores the position of the scrollbar to its state before it was drawn.
			const TSharedRef<SDetailTree> DetailsTree = GetDetailTree();
			DetailsTree->SetScrollOffset(DetailsPanelPrinterParams.ScrollOffset);
		}
		virtual FString GetWidgetTitle() override
		{
			// Do special handling if it contains multiple objects, such as editor preferences.
			if (DetailsPanelPrinterParams.DetailsView->ContainsMultipleTopLevelObjects())
			{
				const FDetailNodeList& RootTreeNodes = GetRootTreeNodes();
				if (RootTreeNodes.IsValidIndex(0))
				{
					const TSharedRef<FDetailTreeNode> RootTreeNode = RootTreeNodes[0];
					if (RootTreeNode->GetNodeType() == EDetailNodeType::Object)
					{
						return RootTreeNode->GetNodeName().ToString();
					}
				}
			}
			
			TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailsPanelPrinterParams.DetailsView->GetSelectedObjects();
			if (SelectedObjects.IsValidIndex(0))
			{
				const TWeakObjectPtr<UObject>& SelectedObject = SelectedObjects[0];
				if (SelectedObject.IsValid())
				{
					// For default objects use the class name to remove the prefix.
					if (SelectedObject->HasAnyFlags(RF_ClassDefaultObject))
					{
						if (const UClass* SelectedObjectClass = SelectedObject->GetClass())
						{
							FString SelectedObjectClassName = SelectedObjectClass->GetName();
							if (SelectedObjectClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
							{
								SelectedObjectClassName.RemoveFromEnd(TEXT("_C"));
							}
							return SelectedObjectClassName;
						}
					}

					// For actors, return the label displayed in the outliner instead of the name of the object.
					if (const auto* SelectedActor = Cast<AActor>(SelectedObject))
					{
						return SelectedActor->GetActorLabel();
					}
							
					return SelectedObject->GetName();
				}
			}

			return TEXT("EmptyDetailsPanel");
		}
		virtual bool WriteWidgetInfoToTextChunk() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
			return true;
#else
			return false;
#endif
		}
		virtual bool RestoreWidgetFromTextChunk() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
			return true;
#else
			return false;
#endif
		}
		virtual bool ShouldAlwaysPrintAll() const override
		{
			return true;
		}
		// End of TInnerWidgetPrinter interface.

	protected:
		// Finds and returns SDetailsView from the searched widget.
		virtual TSharedPtr<SDetailsView> FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const = 0;
		
		// Functions that access protected properties of the DetailsView.
		TSharedRef<SDetailTree> GetDetailTree() const
		{
			const TSharedPtr<SDetailTree> DetailTree = ExtractDetailTree(DetailsPanelPrinterParams.DetailsView.ToSharedRef());
			check(DetailTree.IsValid());
			return DetailTree.ToSharedRef();
		}
		FDetailNodeList& GetRootTreeNodes() const
		{
			return ExtractRootTreeNodes(DetailsPanelPrinterParams.DetailsView.ToSharedRef());
		}
		
	protected:
		// A group of parameters that must be retained for processing.
		struct FDetailsPanelPrinterParams
		{
			// Details view to print.
			TSharedPtr<SDetailsView> DetailsView;

			// Width before pre calculate draw size.
			float PreCalculateDetailsViewWidth = 0.f;
			
			// Details view scrollbar position.
			float ScrollOffset = 0.f;
			
			// Expanded state for each item.
			TMap<FString, bool> ExpansionStateMap;
		};
		FDetailsPanelPrinterParams DetailsPanelPrinterParams;
	};
	
	/**
	 * An inner class with the ability to print and restore details panel.
	 */
	class DETAILSPANELPRINTER_API FDetailsPanelPrinter : public TDetailsPanelPrinter<SDetailsView, UPrintDetailsPanelOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TDetailsPanelPrinter<SDetailsView, UPrintDetailsPanelOptions, URestoreWidgetOptions>;

	public:
		// Constructor.
		FDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SDetailsView> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override;
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override;
		// End of TInnerWidgetPrinter interface.

		// TDetailsPanelPrinter interface.
		virtual TSharedPtr<SDetailsView> FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const override;
		// End of TDetailsPanelPrinter interface.
	};
	
	/**
	 * An inner class with the ability to print and restore actor details panel.
	 */
	class DETAILSPANELPRINTER_API FActorDetailsPanelPrinter : public TDetailsPanelPrinter<SActorDetails, UPrintDetailsPanelOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TDetailsPanelPrinter<SActorDetails, UPrintDetailsPanelOptions, URestoreWidgetOptions>;
		
	public:
		// Constructor.
		FActorDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FActorDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);

		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SActorDetails> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override;
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override;
		// End of TInnerWidgetPrinter interface.

		// TDetailsPanelPrinter interface.
		virtual TSharedPtr<SDetailsView> FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const override;
		// End of TDetailsPanelPrinter interface.
	};
}
