// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "DetailsPanelPrinter/Utilities/DetailsPanelPrinterUtils.h"
#include "EdGraphUtilities.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	template<class TPrintGraphOptions, class TRestoreOptions>
	class TDetailsPanelPrinter
		: public TInnerWidgetPrinter<IDetailsView, TPrintGraphOptions, TRestoreOptions>
	{
	public:
		static_assert(TIsDerivedFrom<TPrintGraphOptions, UPrintDetailsPanelOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintDetailsPanelOptions.");
		
		using Super = TInnerWidgetPrinter<IDetailsView, TPrintGraphOptions, TRestoreOptions>;
		
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

		// TInnerWidgetPrinter interface.
		virtual TSharedPtr<IDetailsView> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			if (SearchTarget.IsValid())
			{
				return FDetailsPanelPrinterUtils::FindNearestChildDetailsView(SearchTarget);
			}
			
			return FDetailsPanelPrinterUtils::GetActiveDetailsView();
		}
		virtual void PreCalculateDrawSize() override
		{
			
		}
		virtual bool CalculateDrawSize(FVector2D& DrawSize) override
		{
			return Super::CalculateDrawSize(DrawSize);
		}
		virtual void PreDrawWidget() override
		{
			
		}
		virtual void PostDrawWidget() override
		{
			
		}
		virtual FString GetWidgetTitle() override
		{
			if (Widget.IsValid())
			{
				TArray<TWeakObjectPtr<UObject>> SelectedObjects = Widget->GetSelectedObjects();
				SelectedObjects.RemoveAll(
					[](const TWeakObjectPtr<UObject>& SelectedObject) -> bool
					{
						return !SelectedObject.IsValid();
					}
				);
				if (SelectedObjects.Num() != 0)
				{
					FString SelectedObjectNames = FString::JoinBy(
						SelectedObjects,
						TEXT("-"),
						[](const TWeakObjectPtr<UObject>& SelectedObject) -> FString
						{
							check(SelectedObject.IsValid());

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
					);

					static constexpr int32 MaxFileNameLength = 50;
					if (SelectedObjectNames.Len() > MaxFileNameLength)
					{
						static const FString MoreString = TEXT("_AndMore");
						SelectedObjectNames.LeftChopInline(MaxFileNameLength - MoreString.Len());
						SelectedObjectNames += MoreString;
					}

					return SelectedObjectNames;
				}
			}
			
			return TEXT("Unknown Object");
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
		// A group of parameters that must be retained for processing.
		struct FDetailsPanelPrinterParams
		{
			
		};
		FDetailsPanelPrinterParams DetailsPanelPrinterParams;
	};

	class DETAILSPANELPRINTER_API FDetailsPanelPrinter : public TDetailsPanelPrinter<UPrintDetailsPanelOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TDetailsPanelPrinter<UPrintDetailsPanelOptions, URestoreWidgetOptions>;
		
	public:
		FDetailsPanelPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FDetailsPanelPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}
	};
}
