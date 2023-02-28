// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "WidgetPrinter/Utilities/CastSlateWidget.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "SDetailsView.h"
#include "SActorDetails.h"
#include "EdGraphUtilities.h"
#include "IDetailRootObjectCustomization.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore details panel.
	 */
	template<class TWidget, class TPrintOptions, class TRestoreOptions>
	class TDetailsPanelPrinter : public TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>
	{
	public:
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintDetailsPanelOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintDetailsPanelOptions.");
		
		using Super = TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::PrintOptions;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::RestoreOptions;
		using TInnerWidgetPrinter<TWidget, TPrintOptions, TRestoreOptions>::Widget;
		
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
			
		}
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override
		{
			DrawSize = Widget->GetDesiredSize();
			return true;
		}
		virtual void PostDrawWidget() override
		{
			
		}
		virtual FString GetWidgetTitle() override
		{
			// Do special handling if it contains multiple objects, such as editor preferences.
			if (DetailsPanelPrinterParams.DetailsView->ContainsMultipleTopLevelObjects())
			{
				// #TODO: I need to get the object that is actually displayed when multiple objects are included and display it.
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
		virtual TSharedPtr<SDetailsView> FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const
		{
			return GP_CAST_SLATE_WIDGET(SDetailsView, SearchTarget);
		}
		
	protected:
		// A group of parameters that must be retained for processing.
		struct FDetailsPanelPrinterParams
		{
			TSharedPtr<SDetailsView> DetailsView;
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
		FDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}
		
		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SDetailsView> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(SearchTarget);
			}
			
			return FDetailsPanelPrinterUtils::GetActiveDetailsView();
		}
		// End of TInnerWidgetPrinter interface.
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
		FActorDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FActorDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}

		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<SActorDetails> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FDetailsPanelPrinterUtils::FindNearestChildActorDetailsView(SearchTarget);
			}
			
			return FDetailsPanelPrinterUtils::GetActiveActorDetailsView();
		}
		// End of TInnerWidgetPrinter interface.

		// TDetailsPanelPrinter interface.
		virtual TSharedPtr<SDetailsView> FindDetailsView(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(SearchTarget);
			}
			
			return FDetailsPanelPrinterUtils::GetActiveDetailsView();
		}
		// End of TDetailsPanelPrinter interface.
	};
}
