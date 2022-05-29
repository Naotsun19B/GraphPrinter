// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "Engine/TextureRenderTarget2D.h"
#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter.generated.h"

namespace GraphPrinter
{
	class IInnerWidgetPrinter;
}

/**
 * A wrapper class that inherits from UObject to collect all existing printers.
 * Please implement the actual processing in the inner class.
 */
UCLASS(Abstract)
class WIDGETPRINTER_API UWidgetPrinter : public UObject
{
	GENERATED_BODY()

public:
	// The rendering result used when the export method is rendered as the render target.
	struct FRenderingResult
	{
	public:
		// The size of the image to output.
		FVector2D DrawSize;

		// A render target that holds the drawing results to be output.
		TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;
		
		// The full path of the output file.
		FString Filename;

	public:
		// Returns whether this result is valid.
		bool IsValid() const
		{
			return (RenderTarget.IsValid() && !Filename.IsEmpty());
		}
	};
	
public:
	// Constructor.
	UWidgetPrinter();
	
	// Draw and export the widget with arguments.
	void PrintWidget(UPrintWidgetOptions* Options);
	
	// Returns whether the target widget can be printed.
	bool CanPrintWidget(UPrintWidgetOptions* Options);
	
	// Restore the state of the widget from the image file.
	void RestoreWidget(URestoreWidgetOptions* Options);

	// Returns whether the target widget can be restored.
	bool CanRestoreWidget(URestoreWidgetOptions* Options);
	
	// Returns the printer priority.
	// Check if the printer can be executed from the one with the highest priority.
	virtual int32 GetPriority() const;
	static int32 GetPriority(const TSubclassOf<UWidgetPrinter>& Class);
	
	// Generates and returns an option class with the default settings applied.
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions(
		const UPrintWidgetOptions::EPrintScope PrintScope,
		const UPrintWidgetOptions::EExportMethod ExportMethod
	) const;
	virtual URestoreWidgetOptions* CreateDefaultRestoreOptions() const;

	// Returns the rendering result used when the export method is rendered as the render target.
	static FRenderingResult GetRenderingResult(
		const TSubclassOf<UWidgetPrinter>& PrinterClass,
		UPrintWidgetOptions* Options
	);

protected:
	// Generates and returns an inner class.
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const;
	virtual TSharedRef<GraphPrinter::IInnerWidgetPrinter> CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const;
	
	// Get optional classes.
	UPrintWidgetOptions* GetPrintOptions() const;
	URestoreWidgetOptions* GetRestoreOptions() const;

	// Discard the cached option class.
	void CleanupPrinter();
	
private:
	// Optional classes that is cached for passing to the inner class.
	UPROPERTY()
	UPrintWidgetOptions* CachedPrintOptions;
	UPROPERTY()
	URestoreWidgetOptions* CachedRestoreOptions;

	// Keep the inner printer running so that it is not destroyed.
	TSharedPtr<GraphPrinter::IInnerWidgetPrinter> InnerPrinter;
};

namespace GraphPrinter
{
	// Generates and returns an option class with the default settings applied.
	template<class TPrinterClass>
	static UPrintWidgetOptions* CreateDefaultPrintOptions(
		const UPrintWidgetOptions::EPrintScope PrintScope,
		const UPrintWidgetOptions::EExportMethod ExportMethod
	)
	{
		static_assert(TIsDerivedFrom<TPrinterClass, UWidgetPrinter>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UWidgetPrinter.");

		if (const UClass* Class = TPrinterClass::StaticClass())
		{
			if (const auto* WidgetPrinter = Cast<UWidgetPrinter>(Class->ClassDefaultObject))
			{
				return WidgetPrinter->CreateDefaultPrintOptions(PrintScope, ExportMethod);
			}
		}

		return nullptr;
	}

	template<class TPrinterClass>
	static URestoreWidgetOptions* CreateDefaultRestoreOptions()
	{
		static_assert(TIsDerivedFrom<TPrinterClass, UWidgetPrinter>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UWidgetPrinter.");

		if (const UClass* Class = TPrinterClass::StaticClass())
		{
			if (const auto* WidgetPrinter = Cast<UWidgetPrinter>(Class->ClassDefaultObject))
			{
				return WidgetPrinter->CreateDefaultRestoreOptions();
			}
		}

		return nullptr;
	}

	// Returns the rendering result used when the export method is rendered as the render target.
	template<class TPrinterClass>
	static UWidgetPrinter::FRenderingResult GetRenderingResult(UPrintWidgetOptions* Options)
	{
		static_assert(TIsDerivedFrom<TPrinterClass, UWidgetPrinter>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UWidgetPrinter.");
		
		return UWidgetPrinter::GetRenderingResult(TPrinterClass::StaticClass(), Options);
	}
}
