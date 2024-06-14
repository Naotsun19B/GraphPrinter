// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "DetailMultiTopLevelObjectRootNode.h"
#include "GameFramework/WorldSettings.h"

#ifdef WITH_DETAILS_PANEL_PRINTER

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
			return Class.*Inner::Pointer; \
		} \
	}

namespace GraphPrinter
{
	namespace Hack
	{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:5103)
#endif
		HACK_INACCESSIBLE_PROPERTY(TSharedPtr<SDetailTree>, SDetailsViewBase, DetailTree)
		HACK_INACCESSIBLE_PROPERTY(FDetailNodeList, SDetailsViewBase, RootTreeNodes)
		HACK_INACCESSIBLE_PROPERTY(FDetailsObjectSet, FDetailMultiTopLevelObjectRootNode, RootObjectSet)
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	}
	
	TSharedPtr<SDetailTree> FDetailsViewBaseAccessor::ExtractDetailTree(const TSharedRef<SDetailsViewBase>& DetailsViewBase)
	{
		return Hack::DetailTree::Extract(DetailsViewBase.Get());
	}
	
	FDetailNodeList& FDetailsViewBaseAccessor::ExtractRootTreeNodes(const TSharedRef<SDetailsViewBase>& DetailsViewBase)
	{
		return Hack::RootTreeNodes::Extract(DetailsViewBase.Get());
	}

	TArray<const UObject*>& FDetailsViewBaseAccessor::ExtractRootObjectSet(const TSharedRef<FDetailTreeNode>& DetailTreeNode)
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
			return FindTargetWidgetFromSearchTarget(SearchTarget);
		}
			
		return FDetailsPanelPrinterUtils::GetActiveDetailsView();
	}

	bool FDetailsPanelPrinter::CalculateDrawSize(FVector2D& DrawSize)
	{
		const bool bSuperResult = Super::CalculateDrawSize(DrawSize);

		// There is a difference in the height of the drawing size, so adjusts by adding the height of one item.
		TOptional<float> ItemHeight;
		{
			const TSharedRef<SDetailTree> DetailsTree = GetDetailTree(DetailsPanelPrinterParams.DetailsView);
			const FDetailNodeList& RootTreeNodes = GetRootTreeNodes(DetailsPanelPrinterParams.DetailsView);
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

	FString FDetailsPanelPrinter::GetWidgetTitle()
	{
		return GetEditingObjectName(DetailsPanelPrinterParams.DetailsView);
	}

	TSharedPtr<SDetailsView> FDetailsPanelPrinter::FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const
	{
		return GP_CAST_SLATE_WIDGET(SDetailsView, SearchTarget);
	}

	bool FDetailsPanelPrinter::SupportsEditingObjectClass(const UClass* EditingObjectClass) const
	{
		if (IsValid(EditingObjectClass))
		{
			// Applies to classes that inherit AActor if the instance is the default object.
			if (EditingObjectClass->IsChildOf<AActor>() && EditingObjectClass->IsTemplate())
			{
				return true;
			}
			
			return EditingObjectClass->IsChildOf<UObject>();
		}

		return false;
	}

	TSharedPtr<SDetailsView> FDetailsPanelPrinter::FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget)
	{
		const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(SearchTarget);
		return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(DockingTabStack);
	}

	FString FDetailsPanelPrinter::GetEditingObjectName(const TSharedPtr<SDetailsView>& DetailsPanel)
	{
		UObject* EditingObject = GetSingleEditingObject(DetailsPanel);
		if (!IsValid(EditingObject))
		{
			return TEXT("EmptyDetailsPanel");
		}
			
		// For default objects uses the class name to remove the suffix.
		FString EditingObjectName = EditingObject->GetName();
		if (EditingObject->HasAnyFlags(RF_ClassDefaultObject))
		{
			if (const UClass* SelectedObjectClass = EditingObject->GetClass())
			{
				FString SelectedObjectClassName = SelectedObjectClass->GetName();
				if (SelectedObjectClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
				{
					SelectedObjectClassName.RemoveFromEnd(TEXT("_C"));
				}
				EditingObjectName = SelectedObjectClassName;
			}
		}
							
		return FString::Printf(TEXT("Details-%s"), *EditingObjectName);
	}

	FActorDetailsPanelPrinter::FActorDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FActorDetailsPanelPrinter::FActorDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	bool FActorDetailsPanelPrinter::CalculateDrawSize(FVector2D& DrawSize)
	{
		const bool bSuperResult = Super::CalculateDrawSize(DrawSize);

		// Since the actor detail panel has a different structure than other detail panels,
		// adds the difference between the displayed size of subobject instance editor and details view and the actual size.
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

	FString FActorDetailsPanelPrinter::GetWidgetTitle()
	{
		return GetEditingActorName(DetailsPanelPrinterParams.DetailsView);
	}

	TSharedPtr<SActorDetails> FActorDetailsPanelPrinter::FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
	{
		if (SearchTarget.IsValid())
		{
			return FindTargetWidgetFromSearchTarget(SearchTarget);
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
			// Applies to objects that inherit the AActor class and whose instance is not the default object.
			return (EditingObjectClass->IsChildOf<AActor>() && !EditingObjectClass->IsTemplate());
		}

		return false;
	}

	TSharedPtr<SActorDetails> FActorDetailsPanelPrinter::FindTargetWidgetFromSearchTarget(const TSharedPtr<SWidget>& SearchTarget)
	{
		const TSharedPtr<SWidget> DockingTabStack = FWidgetPrinterUtils::FindNearestParentDockingTabStack(SearchTarget);
		return FDetailsPanelPrinterUtils::FindNearestChildActorDetailsView(DockingTabStack);
	}

	FString FActorDetailsPanelPrinter::GetEditingActorName(const TSharedPtr<SDetailsView>& DetailsPanel)
	{
		UObject* EditingObject = GetSingleEditingObject(DetailsPanel);
		if (!IsValid(EditingObject))
		{
			return TEXT("EmptyActorDetailsPanel");
		}
		
		// For actors, returns the label displayed in the outliner instead of the name of the object.
		FString EditingActorName = EditingObject->GetName();
		if (const auto* SelectedActor = Cast<AActor>(EditingObject))
		{
			EditingActorName = SelectedActor->GetActorLabel();
		}
		
		return FString::Printf(TEXT("ActorDetails-%s"), *EditingActorName);
	}
}

#undef HACK_INACCESSIBLE_PROPERTY
#endif
