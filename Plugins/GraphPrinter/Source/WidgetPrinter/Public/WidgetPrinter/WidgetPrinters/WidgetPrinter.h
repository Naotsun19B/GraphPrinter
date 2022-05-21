// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WidgetPrinter/Types/PrintWidgetOptions.h"
#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter.generated.h"

namespace GraphPrinter
{
	class IInnerPrinter;
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
	template<class TPrinterClass>
	static int32 GetPriority()
	{
		static_assert(TIsDerivedFrom<TPrinterClass, UWidgetPrinter>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UWidgetPrinter.");

		if (const UClass* Class = TPrinterClass::StaticClass())
		{
			if (const auto* WidgetPrinter = Cast<UWidgetPrinter>(Class->ClassDefaultObject))
			{
				return WidgetPrinter->GetPriority();
			}
		}

		return INDEX_NONE;
	}

	// Generates and returns an option class with the default settings applied.
	virtual UPrintWidgetOptions* CreateDefaultPrintOptions() const;
	virtual URestoreWidgetOptions* CreateDefaultRestoreOptions() const;
	
protected:
	// Generates and returns an inner class.
	virtual TSharedRef<GraphPrinter::IInnerPrinter> CreateInnerPrinter() const;
	
	// Get optional classes.
	UPrintWidgetOptions* GetPrintOptions() const;
	template<class TPrintOptions>
	TPrintOptions* GetPrintOptions() const
	{
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintWidgetOptions.");
		return Cast<TPrintOptions>(GetPrintOptions());
	}
	URestoreWidgetOptions* GetRestoreOptions() const;
	template<class TRestoreOptions>
	TRestoreOptions* GetRestoreOptions() const
	{
		static_assert(TIsDerivedFrom<TRestoreOptions, URestoreWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of URestoreWidgetOptions.");
		return Cast<TRestoreOptions>(GetRestoreOptions());
	}

	// Discard the cached option class.
	void CleanupPrintOptions();
	void CleanupRestoreOptions();
	
private:
	// Optional classes that is cached for passing to the inner class.
	UPROPERTY()
	UPrintWidgetOptions* CachedPrintOptions;
	UPROPERTY()
	URestoreWidgetOptions* CachedRestoreOptions;
};
