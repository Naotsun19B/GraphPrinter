// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "DetailsPanelPrinter/Types/PrintDetailsPanelOptions.h"
#include "EdGraphUtilities.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	template<class TPrintGraphOptions, class TRestoreOptions>
	class TDetailsPanelPrinter
		: public TInnerWidgetPrinter<SWidget, TPrintGraphOptions, TRestoreOptions>
	{
	public:
		static_assert(TIsDerivedFrom<TPrintGraphOptions, UPrintDetailsPanelOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintDetailsPanelOptions.");
		
		using Super = TInnerWidgetPrinter<SWidget, TPrintGraphOptions, TRestoreOptions>;
		
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
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget())
			{
				
			}

			return false;
		}
		virtual bool CanRestoreWidget() const override
		{
			return Super::CanRestoreWidget();
		}
		virtual TSharedPtr<SWidget> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const override
		{
			return Super::FindTargetWidget(SearchTarget);
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
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override
		{
			

			UTextureRenderTarget2D* RenderTarget = Super::DrawWidgetToRenderTarget();

			
	
			return RenderTarget;
		}
		virtual void PostDrawWidget() override
		{
			
		}
		virtual FString GetWidgetTitle() override
		{
			return Super::GetWidgetTitle();
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
