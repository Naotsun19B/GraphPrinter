// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Types/RestoreDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "SDetailsView.h"
#include "SActorDetails.h"
#include "EdGraphUtilities.h"

#ifdef WITH_TEXT_CHUNK_HELPER
#include "TextChunkHelper/ITextChunkHelper.h"
#endif

#ifdef WITH_DETAILS_PANEL_PRINTER
namespace GraphPrinter
{
#ifdef WITH_TEXT_CHUNK_HELPER
	namespace DetailsPanelPrinter
	{
		namespace TextChunkDefine
		{
			// Key used when writing to a text chunk of a png file.
			static const FString PropertiesChunkKey = TEXT("DetailsPanel-Properties");
			static const FString ExpansionStatesChunkKey = TEXT("DetailsPanel-ExpansionStates");

			// The beginning of the widget information.
			static const FString PropertiesInfoHeader = TEXT("Properties");
			static const FString ExpansionStatesInfoHeader = TEXT("ExpansionStates");
		}
	}
#endif
	
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
		static TArray<const UObject*>& ExtractRootObjectSet(TSharedRef<FDetailTreeNode> DetailTreeNode);
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
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget() && IsValid(PrintOptions))
			{
				const TSharedPtr<SDetailsView> DetailsView = FindDetailsView(FindTargetWidget(PrintOptions->SearchTarget));
				if (DetailsView.IsValid())
				{
					if (const UObject* EditingObject = GetSingleEditingObject(DetailsView))
					{
						if (const UClass* EditingObjectClass = EditingObject->GetClass())
						{
							return SupportsEditingObjectClass(EditingObjectClass);
						}
					}
				}
			}

			return false;
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
					if (const UObject* EditingObject = GetSingleEditingObject(DetailsView))
					{
						if (const UClass* EditingObjectClass = EditingObject->GetClass())
						{
							if (SupportsEditingObjectClass(EditingObjectClass))
							{
								return DetailsView->IsPropertyEditingEnabled();
							}
						}
					}
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
			UObject* EditingObject = GetSingleEditingObject();
			if (!IsValid(EditingObject))
			{
				return TEXT("EmptyDetailsPanel");
			}
			
			// For default objects use the class name to remove the prefix.
			if (EditingObject->HasAnyFlags(RF_ClassDefaultObject))
			{
				if (const UClass* SelectedObjectClass = EditingObject->GetClass())
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
			if (const auto* SelectedActor = Cast<AActor>(EditingObject))
			{
				return SelectedActor->GetActorLabel();
			}
							
			return EditingObject->GetName();
		}
		virtual bool WriteWidgetInfoToTextChunk() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
			TMap<FString, FString> MapToWrite;
			{
				UObject* EditingObject = GetSingleEditingObject();
				if (!IsValid(EditingObject))
				{
					return false;
				}

				FString PropertiesJsonString;
				if (!FDetailsPanelPrinterUtils::ExportObjectPropertiesAsJsonString(EditingObject, PropertiesJsonString))
				{
					return false;
				}
				PropertiesJsonString = DetailsPanelPrinter::TextChunkDefine::PropertiesInfoHeader + PropertiesJsonString;
				MapToWrite.Add(DetailsPanelPrinter::TextChunkDefine::PropertiesChunkKey, PropertiesJsonString);
			}
			if (PrintOptions->bIsIncludeExpansionStateInImageFile)
			{
				FString ExpansionStatesString;
				for (const auto& Pair : DetailsPanelPrinterParams.ExpansionStateMap)
				{
					const FString PropertyNodePath = Pair.Key;
					const bool bIsExpanded = Pair.Value;
					ExpansionStatesString += FString::Printf(TEXT("%s:%d,"), *PropertyNodePath, static_cast<int32>(bIsExpanded));
				}
				ExpansionStatesString = DetailsPanelPrinter::TextChunkDefine::ExpansionStatesInfoHeader + ExpansionStatesString;
				MapToWrite.Add(DetailsPanelPrinter::TextChunkDefine::ExpansionStatesChunkKey, ExpansionStatesString);
			}

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

			// Applies read property data to object properties.
			{
				if (!MapToRead.Contains(DetailsPanelPrinter::TextChunkDefine::PropertiesChunkKey))
				{
					return false;
				}

				FString PropertiesJsonString = MapToRead[DetailsPanelPrinter::TextChunkDefine::PropertiesChunkKey];

				// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
				FGraphPrinterUtils::ClearUnnecessaryCharactersFromHead(
					PropertiesJsonString,
					DetailsPanelPrinter::TextChunkDefine::PropertiesInfoHeader
				);
				const int32 PropertiesInfoHeaderLength = DetailsPanelPrinter::TextChunkDefine::PropertiesInfoHeader.Len();
				PropertiesJsonString = PropertiesJsonString.Mid(
					PropertiesInfoHeaderLength,
					PropertiesJsonString.Len() - PropertiesInfoHeaderLength
				);

				UObject* EditingObject = GetSingleEditingObject();
				if (!IsValid(EditingObject))
				{
					return false;
				}
			
				if (!FDetailsPanelPrinterUtils::ImportObjectPropertiesFromJsonString(EditingObject, PropertiesJsonString))
				{
					return false;
				}
			}

			// Applies read expansion states data to details panel expansion states.
			if (RestoreOptions->bWhetherToAlsoRestoreExpandedStates &&
				MapToRead.Contains(DetailsPanelPrinter::TextChunkDefine::ExpansionStatesChunkKey))
			{
				FString ExpansionStatesString = MapToRead[DetailsPanelPrinter::TextChunkDefine::ExpansionStatesChunkKey];
				
				// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
				FGraphPrinterUtils::ClearUnnecessaryCharactersFromHead(
					ExpansionStatesString,
					DetailsPanelPrinter::TextChunkDefine::ExpansionStatesInfoHeader
				);
				const int32 ExpansionStatesInfoHeaderLength = DetailsPanelPrinter::TextChunkDefine::ExpansionStatesInfoHeader.Len();
				ExpansionStatesString = ExpansionStatesString.Mid(
					ExpansionStatesInfoHeaderLength,
					ExpansionStatesString.Len() - ExpansionStatesInfoHeaderLength
				);

				TArray<FString> ExpansionStatePairsString;
				ExpansionStatesString.ParseIntoArray(ExpansionStatePairsString, TEXT(","));

				DetailsPanelPrinterParams.ExpansionStateMap.Reserve(ExpansionStatePairsString.Num());
				
				for (const auto& ExpansionStatePairString : ExpansionStatePairsString)
				{
					FString PropertyNodePath;
					FString IsExpandedString;
					if (!ExpansionStatePairString.Split(TEXT(":"), &PropertyNodePath, &IsExpandedString))
					{
						continue;
					}

					const bool bIsExpanded = (FCString::Atoi(*IsExpandedString) > 0);
					DetailsPanelPrinterParams.ExpansionStateMap.Add(PropertyNodePath, bIsExpanded);
				}

				const TSharedRef<SDetailTree> DetailsTree = GetDetailTree();
				TFunction<void(FDetailNodeList&, const FString&)> ApplyExpansionStateRecursive =
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
							
							if (const bool* IsExpandedPtr = DetailsPanelPrinterParams.ExpansionStateMap.Find(CombinedNodeName))
							{
								DetailsTree->SetItemExpansion(Node, *IsExpandedPtr);
							}

							FDetailNodeList Children;
							Node->GetChildren(Children);
							ApplyExpansionStateRecursive(Children, NodeName);
						}
					};

				FDetailNodeList& RootTreeNodes = GetRootTreeNodes();
				ApplyExpansionStateRecursive(RootTreeNodes, TEXT(""));

				DetailsTree->RequestTreeRefresh();
			}

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

		// Returns whether the class of the object being edited supports this printer.
		virtual bool SupportsEditingObjectClass(const UClass* EditingObjectClass) const = 0;
		
		// Returns one object that the details view is editing.
		UObject* GetSingleEditingObject(TSharedPtr<SDetailsViewBase> DetailsViewBase = nullptr) const
		{
			if (!DetailsViewBase.IsValid())
			{
				DetailsViewBase = DetailsPanelPrinterParams.DetailsView;
			}
			check(DetailsViewBase.IsValid());
			
			// Do special handling if it contains multiple objects, such as editor preferences.
			if (DetailsViewBase->ContainsMultipleTopLevelObjects())
			{
				TArray<UObject*> RootObjectSet = GetRootObjectSet(DetailsViewBase);
				if (RootObjectSet.IsValidIndex(0))
				{
					return RootObjectSet[0];
				}
			}
			else
			{
				TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailsViewBase->GetSelectedObjects();
				if (SelectedObjects.IsValidIndex(0))
				{
					return SelectedObjects[0].Get();
				}
			}

			return nullptr;
		}
		
		// Functions that access protected properties of the DetailsView.
		TSharedRef<SDetailTree> GetDetailTree(TSharedPtr<SDetailsViewBase> DetailsViewBase = nullptr) const
		{
			if (!DetailsViewBase.IsValid())
			{
				DetailsViewBase = DetailsPanelPrinterParams.DetailsView;
			}
			check(DetailsViewBase.IsValid());
			
			const TSharedPtr<SDetailTree> DetailTree = ExtractDetailTree(DetailsViewBase.ToSharedRef());
			check(DetailTree.IsValid());
			return DetailTree.ToSharedRef();
		}
		FDetailNodeList& GetRootTreeNodes(TSharedPtr<SDetailsViewBase> DetailsViewBase = nullptr) const
		{
			if (!DetailsViewBase.IsValid())
			{
				DetailsViewBase = DetailsPanelPrinterParams.DetailsView;
			}
			check(DetailsViewBase.IsValid());
			
			return ExtractRootTreeNodes(DetailsViewBase.ToSharedRef());
		}
		TArray<UObject*> GetRootObjectSet(TSharedPtr<SDetailsViewBase> DetailsViewBase = nullptr) const
		{
			TArray<UObject*> RootObjectSet;
			
			FDetailNodeList& RootTreeNodes = GetRootTreeNodes(DetailsViewBase);
			for (const TSharedRef<FDetailTreeNode>& RootTreeNode : RootTreeNodes)
			{
				TArray<const UObject*> ConstRootObjectSet = ExtractRootObjectSet(RootTreeNode);
				RootObjectSet.Reserve(RootObjectSet.Num() + RootTreeNodes.Num());
				for (const auto* ConstRootObject : ConstRootObjectSet)
				{
					RootObjectSet.Add(const_cast<UObject*>(ConstRootObject));
				}
			}
			
			return RootObjectSet;
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
	class DETAILSPANELPRINTER_API FDetailsPanelPrinter : public TDetailsPanelPrinter<SDetailsView, UPrintDetailsPanelOptions, URestoreDetailsPanelOptions>
	{
	public:
		using Super = TDetailsPanelPrinter<SDetailsView, UPrintDetailsPanelOptions, URestoreDetailsPanelOptions>;

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
		virtual bool SupportsEditingObjectClass(const UClass* EditingObjectClass) const override;
		// End of TDetailsPanelPrinter interface.
	};
	
	/**
	 * An inner class with the ability to print and restore actor details panel.
	 */
	class DETAILSPANELPRINTER_API FActorDetailsPanelPrinter : public TDetailsPanelPrinter<SActorDetails, UPrintDetailsPanelOptions, URestoreDetailsPanelOptions>
	{
	public:
		using Super = TDetailsPanelPrinter<SActorDetails, UPrintDetailsPanelOptions, URestoreDetailsPanelOptions>;
		
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
		virtual bool SupportsEditingObjectClass(const UClass* EditingObjectClass) const override;
		// End of TDetailsPanelPrinter interface.
	};
}
#endif
