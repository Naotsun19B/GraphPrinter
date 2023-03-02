// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "DetailsPanelPrinter/WidgetPrinters/InnerDetailsPanelPrinter.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"

namespace GraphPrinter
{
	namespace DetailsViewBaseAccessor
	{
		namespace DetailTree
		{
			static TSharedPtr<SDetailTree> SDetailsViewBase::* Pointer;
		
			template<typename Tag, typename Tag::Type Ptr>
			struct THacker
			{
				struct FFiller
				{
					FFiller()
					{
						Pointer = Ptr;
					}
				};
				static FFiller Filler;
			};

			template<typename Tag, typename Tag::Type Ptr>
			typename THacker<Tag, Ptr>::FFiller THacker<Tag, Ptr>::Filler;

			struct FRootTreeNodesTag
			{
				typedef TSharedPtr<SDetailTree> SDetailsViewBase::*Type;
			};
			template struct THacker<FRootTreeNodesTag, &SDetailsViewBase::DetailTree>;
		}
		
		namespace RootTreeNodes
		{
			static FDetailNodeList SDetailsViewBase::* Pointer;
		
			template<typename Tag, typename Tag::Type Ptr>
			struct THacker
			{
				struct FFiller
				{
					FFiller()
					{
						Pointer = Ptr;
					}
				};
				static FFiller Filler;
			};

			template<typename Tag, typename Tag::Type Ptr>
			typename THacker<Tag, Ptr>::FFiller THacker<Tag, Ptr>::Filler;

			struct FRootTreeNodesTag
			{
				typedef FDetailNodeList SDetailsViewBase::*Type;
			};
			template struct THacker<FRootTreeNodesTag, &SDetailsViewBase::RootTreeNodes>;
		}
	}
	
	TSharedPtr<SDetailTree> FDetailsViewBaseAccessor::ExtractDetailTree(TSharedRef<SDetailsViewBase> DetailsViewBase)
	{
		return DetailsViewBase.Get().*DetailsViewBaseAccessor::DetailTree::Pointer;
	}
	
	FDetailNodeList& FDetailsViewBaseAccessor::ExtractRootTreeNodes(TSharedRef<SDetailsViewBase> DetailsViewBase)
	{
		return DetailsViewBase.Get().*DetailsViewBaseAccessor::RootTreeNodes::Pointer;
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

	float FActorDetailsPanelPrinter::GetAdditionalDrawHeight() const
	{
		float SubobjectInstanceEditorHeightDifference = 0.f;
		
		const TSharedPtr<SWidget> SubobjectInstanceEditor = FDetailsPanelPrinterUtils::FindNearestChildSubobjectInstanceEditor(Widget);
		if (SubobjectInstanceEditor.IsValid())
		{
			const FGeometry& Geometry =
#if UE_4_24_OR_LATER
				SubobjectInstanceEditor->GetTickSpaceGeometry();
#else
				SubobjectInstanceEditor->GetCachedGeometry();
#endif
			const FVector2D LocalSize = Geometry.GetLocalSize();
			const FVector2D DesiredSize = SubobjectInstanceEditor->GetDesiredSize();

			SubobjectInstanceEditorHeightDifference = FMath::Abs(LocalSize.Y - DesiredSize.Y);
			SubobjectInstanceEditorHeightDifference *= 5.f;
		}
		
		return (Super::GetAdditionalDrawHeight() + SubobjectInstanceEditorHeightDifference);
	}
}
