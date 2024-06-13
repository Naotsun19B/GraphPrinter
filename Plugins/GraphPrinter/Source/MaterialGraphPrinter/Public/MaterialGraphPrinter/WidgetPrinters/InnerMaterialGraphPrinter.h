// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"
#include "MaterialGraphPrinter/Types/PrintMaterialGraphOptions.h"

class UTextureRenderTarget2D;

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class MATERIALGRAPHPRINTER_API FMaterialGraphPrinter
		: public TGraphPrinter<UPrintMaterialGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintMaterialGraphOptions, URestoreWidgetOptions>;

	public:
		// Constructor.
		FMaterialGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		FMaterialGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished);
		
		// IInnerWidgetPrinter interface.
		virtual bool CanPrintWidget() const override;
		// End of IInnerWidgetPrinter interface.

		// TInnerWidgetPrinter interface.
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override;
		virtual FString GetWidgetTitle() override;
		// End of TInnerWidgetPrinter interface.
		
		// Returns the title from the material graph in the format "[material name]-[graph title]".
		static bool GetMaterialGraphTitle(const TSharedPtr<SGraphEditorImpl>& MaterialGraphEditor, FString& Title);
	};
}
