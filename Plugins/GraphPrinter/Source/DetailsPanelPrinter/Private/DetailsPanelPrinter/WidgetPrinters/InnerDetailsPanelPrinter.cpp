// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "DetailMultiTopLevelObjectRootNode.h"
#include "GameFramework/WorldSettings.h"

#define HACK_INACCESSIBLE_PROPERTY(PropertyType, ClassType, PropertyName) \
	namespace PropertyName \
	{ \
		namespace Inner \
		{ \
			static PropertyType ClassType##::* Pointer; \
			\
			template<typename Tag, typename Tag::Type Ptr> \
			struct THacker \
			{ \
				struct FFiller \
				{ \
					FFiller() \
					{ \
						Pointer = Ptr; \
					} \
				}; \
				static FFiller Filler; \
			}; \
			\
			template<typename Tag, typename Tag::Type Ptr> \
			typename THacker<Tag, Ptr>::FFiller THacker<Tag, Ptr>::Filler; \
			\
			struct FTag \
			{ \
				typedef PropertyType ClassType##::*Type; \
			}; \
			template struct THacker<FTag, &##ClassType##::##PropertyName##>; \
		} \
		\
		static PropertyType& Extract(ClassType& Class) \
		{ \
		return Class.*Inner::Pointer;\
		} \
	}

namespace GraphPrinter
{
	namespace Hack
	{
		HACK_INACCESSIBLE_PROPERTY(TSharedPtr<SDetailTree>, SDetailsViewBase, DetailTree)
		HACK_INACCESSIBLE_PROPERTY(FDetailNodeList, SDetailsViewBase, RootTreeNodes)
		HACK_INACCESSIBLE_PROPERTY(FDetailsObjectSet, FDetailMultiTopLevelObjectRootNode, RootObjectSet)
	}
	
	TSharedPtr<SDetailTree> FDetailsViewBaseAccessor::ExtractDetailTree(TSharedRef<SDetailsViewBase> DetailsViewBase)
	{
		return Hack::DetailTree::Extract(DetailsViewBase.Get());
	}
	
	FDetailNodeList& FDetailsViewBaseAccessor::ExtractRootTreeNodes(TSharedRef<SDetailsViewBase> DetailsViewBase)
	{
		return Hack::RootTreeNodes::Extract(DetailsViewBase.Get());
	}

	TArray<const UObject*>& FDetailsViewBaseAccessor::ExtractRootObjectSet(TSharedRef<FDetailTreeNode> DetailTreeNode)
	{
		check(DetailTreeNode->GetNodeType() == EDetailNodeType::Object);

		const TSharedRef<FDetailMultiTopLevelObjectRootNode> DetailMultiTopLevelObjectRootNode = StaticCastSharedRef<FDetailMultiTopLevelObjectRootNode>(DetailTreeNode);
		FDetailsObjectSet& DetailsObjects = Hack::RootObjectSet::Extract(DetailMultiTopLevelObjectRootNode.Get());
		return DetailsObjects.RootObjects;
	}

	FDetailsPanelPrinter::FDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FDetailsPanelPrinter::FDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	TSharedPtr<SDetailsView> FDetailsPanelPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
	{
		if (SearchTarget.IsValid())
		{
			return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(SearchTarget);
		}
			
		return FDetailsPanelPrinterUtils::GetActiveDetailsView();
	}

	bool FDetailsPanelPrinter::CalculateDrawSize(FVector2D& DrawSize)
	{
		const bool bSuperResult = Super::CalculateDrawSize(DrawSize);

		// There is a difference in the height of the drawing size, so adjust by adding the height of one item.
		TOptional<float> ItemHeight;
		{
			const TSharedRef<SDetailTree> DetailsTree = GetDetailTree();
			const FDetailNodeList& RootTreeNodes = GetRootTreeNodes();
			if (RootTreeNodes.IsValidIndex(0))
			{
				const TSharedPtr<ITableRow> TableRow = DetailsTree->WidgetFromItem(RootTreeNodes[0]);
				if (TableRow.IsValid())
				{
					const TSharedRef<SWidget> ItemWidget = TableRow->AsWidget();
					ItemHeight = ItemWidget->GetDesiredSize().Y;
				}
			}
		}
		if (ItemHeight.IsSet())
		{
			DrawSize.Y += ItemHeight.GetValue();
			return bSuperResult;
		}

		return false;
	}

	TSharedPtr<SDetailsView> FDetailsPanelPrinter::FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const
	{
		return GP_CAST_SLATE_WIDGET(SDetailsView, SearchTarget);
	}

	bool FDetailsPanelPrinter::SupportsEditingObjectClass(const UClass* EditingObjectClass) const
	{
		if (IsValid(EditingObjectClass))
		{
			return EditingObjectClass->IsChildOf<UObject>();
		}

		return false;
	}

	FActorDetailsPanelPrinter::FActorDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FActorDetailsPanelPrinter::FActorDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	TSharedPtr<SActorDetails> FActorDetailsPanelPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
	{
		if (SearchTarget.IsValid())
		{
			return FDetailsPanelPrinterUtils::FindNearestChildActorDetailsView(SearchTarget);
		}
			
		return FDetailsPanelPrinterUtils::GetActiveActorDetailsView();
	}

	TSharedPtr<SDetailsView> FActorDetailsPanelPrinter::FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const
	{
		if (SearchTarget.IsValid())
		{
			return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(SearchTarget);
		}
			
		return FDetailsPanelPrinterUtils::GetActiveDetailsView();
	}

	bool FActorDetailsPanelPrinter::SupportsEditingObjectClass(const UClass* EditingObjectClass) const
	{
		if (IsValid(EditingObjectClass))
		{
			return (
				EditingObjectClass->IsChildOf<AActor>() &&
				// AWorldSettings is a class that inherits from AActor, but exclude it since it will be using the normal details panel.
				!EditingObjectClass->IsChildOf<AWorldSettings>()
			);
		}

		return false;
	}

	bool FActorDetailsPanelPrinter::CalculateDrawSize(FVector2D& DrawSize)
	{
		const bool bSuperResult = Super::CalculateDrawSize(DrawSize);

		// Since the actor detail panel has a different structure than other detail panels,
		// add the difference between the displayed size of subobject instance editor and details view and the actual size.
		const TSharedPtr<SWidget> SubobjectInstanceEditor = FDetailsPanelPrinterUtils::FindNearestChildSubobjectInstanceEditor(Widget);
		if (!SubobjectInstanceEditor.IsValid())
		{
			return false;
		}
		DrawSize.Y += FDetailsPanelPrinterUtils::GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(SubobjectInstanceEditor).Y;
		
		// #TODO: It is necessary to investigate the reason why the accurate size cannot be measured.
		// If don't apply a fixed scale here, the image will be too large.
		static constexpr float DetailsViewDifferenceScale = 0.3f;
		DrawSize.Y += FDetailsPanelPrinterUtils::GetDifferenceBetweenWidgetLocalSizeAndDesiredSize(DetailsPanelPrinterParams.DetailsView).Y * DetailsViewDifferenceScale;
		
		return bSuperResult;
	}
}

#undef HACK_INACCESSIBLE_PROPERTY
