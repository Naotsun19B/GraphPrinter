// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "WidgetPrinter/WidgetPrinters/InnerPrinter.h"
#include "WidgetPrinter/Utilities/GraphPrinterSettings.h"

namespace GraphPrinter
{
	class FWidgetPrinter : public TInnerPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>;
		
	public:
		FWidgetPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FWidgetPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}
	};
}

UWidgetPrinter::UWidgetPrinter()
{
	if (!IsTemplate())
	{
		// Temporarily exclude the instance from garbage collection to prevent
		// the instance from being destroyed during printer processing.
		AddToRoot();
	}
}

void UWidgetPrinter::PrintWidget(UPrintWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedPrintOptions = Options;

	InnerPrinter = CreatePrintModeInnerPrinter(
		FSimpleDelegate::CreateUObject(this, &UWidgetPrinter::CleanupPrinter)
	);
	InnerPrinter->PrintWidget();
}

bool UWidgetPrinter::CanPrintWidget(UPrintWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedPrintOptions = Options;

	InnerPrinter = CreatePrintModeInnerPrinter(
		FSimpleDelegate::CreateUObject(this, &UWidgetPrinter::CleanupPrinter)
	);
	return InnerPrinter->CanPrintWidget();
}

void UWidgetPrinter::RestoreWidget(URestoreWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedRestoreOptions = Options;

	InnerPrinter = CreateRestoreModeInnerPrinter(
		FSimpleDelegate::CreateUObject(this, &UWidgetPrinter::CleanupPrinter)
	);
	InnerPrinter->RestoreWidget();
}

bool UWidgetPrinter::CanRestoreWidget(URestoreWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedRestoreOptions = Options;

	InnerPrinter = CreateRestoreModeInnerPrinter(
		FSimpleDelegate::CreateUObject(this, &UWidgetPrinter::CleanupPrinter)
	);
	return InnerPrinter->CanRestoreWidget();
}

int32 UWidgetPrinter::GetPriority() const
{
	return TNumericLimits<int32>::Min();
}

int32 UWidgetPrinter::GetPriority(const TSubclassOf<UWidgetPrinter>& Class)
{
	if (IsValid(Class))
	{
		if (const auto* WidgetPrinter = Cast<UWidgetPrinter>(Class->ClassDefaultObject))
		{
			return WidgetPrinter->GetPriority();
		}
	}

	return TNumericLimits<int32>::Min();
}

UPrintWidgetOptions* UWidgetPrinter::CreateDefaultPrintOptions() const
{
	auto* PrintOptions = NewObject<UPrintWidgetOptions>();
	if (IsValid(PrintOptions))
	{
		auto& Settings = UGraphPrinterSettings::Get();

#ifdef WITH_TEXT_CHUNK_HELPER
		PrintOptions->bIsIncludeNodeInfoInImageFile = Settings.bIsIncludeNodeInfoInImageFile;
#endif
		PrintOptions->ImageWriteOptions.Format = Settings.Format;
		PrintOptions->ImageWriteOptions.CompressionQuality = Settings.CompressionQuality;
		PrintOptions->FilteringMode = Settings.FilteringMode;
		PrintOptions->bUseGamma = Settings.bUseGamma;
		PrintOptions->Padding = Settings.Padding;
		PrintOptions->MaxImageSize = Settings.MaxImageSize;
		PrintOptions->RenderingScale = Settings.RenderingScale;
		PrintOptions->ImageWriteOptions.bOverwriteFile = Settings.bCanOverwriteFileWhenExport;
		PrintOptions->OutputDirectoryPath = Settings.OutputDirectory.Path;
	}
	
	return PrintOptions;
}

URestoreWidgetOptions* UWidgetPrinter::CreateDefaultRestoreOptions() const
{
	auto* RestoreOptions = NewObject<URestoreWidgetOptions>();
	return RestoreOptions;
}

TSharedRef<GraphPrinter::IInnerPrinter> UWidgetPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FWidgetPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerPrinter> UWidgetPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FWidgetPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}

UPrintWidgetOptions* UWidgetPrinter::GetPrintOptions() const
{
	return CachedPrintOptions;
}

URestoreWidgetOptions* UWidgetPrinter::GetRestoreOptions() const
{
	return CachedRestoreOptions;
}

void UWidgetPrinter::CleanupPrinter()
{
	CachedPrintOptions = nullptr;
	CachedRestoreOptions = nullptr;
	InnerPrinter.Reset();

	// Since it is no longer necessary to keep an instance of the printer, it is subject to garbage collection.
	RemoveFromRoot();
}