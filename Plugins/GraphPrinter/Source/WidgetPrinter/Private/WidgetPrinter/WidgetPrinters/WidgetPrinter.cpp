// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "WidgetPrinter/WidgetPrinters/InnerPrinter.h"

namespace GraphPrinter
{
	class FWidgetPrinter : public TInnerPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		FWidgetPrinter(UPrintWidgetOptions* InPrintOptions, URestoreWidgetOptions* InRestoreOptions)
			: TInnerPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>(InPrintOptions, InRestoreOptions)
		{
		}
	};
}

void UWidgetPrinter::PrintWidget(UPrintWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedPrintOptions = Options;

	const TSharedRef<GraphPrinter::IInnerPrinter> InnerPrinter = CreateInnerPrinter();
	InnerPrinter->PrintWidget();
}

bool UWidgetPrinter::CanPrintWidget(UPrintWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedPrintOptions = Options;

	const TSharedRef<GraphPrinter::IInnerPrinter> InnerPrinter = CreateInnerPrinter();
	return InnerPrinter->CanPrintWidget();
}

void UWidgetPrinter::RestoreWidget(URestoreWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedRestoreOptions = Options;

	const TSharedRef<GraphPrinter::IInnerPrinter> InnerPrinter = CreateInnerPrinter();
	InnerPrinter->RestoreWidget();
}

bool UWidgetPrinter::CanRestoreWidget(URestoreWidgetOptions* Options)
{
	check(IsValid(Options));
	CachedRestoreOptions = Options;

	const TSharedRef<GraphPrinter::IInnerPrinter> InnerPrinter = CreateInnerPrinter();
	return InnerPrinter->CanRestoreWidget();
}

int32 UWidgetPrinter::GetPriority() const
{
	return TNumericLimits<int32>::Min();
}

UPrintWidgetOptions* UWidgetPrinter::CreateDefaultPrintOptions() const
{
	auto* PrintOptions = NewObject<UPrintWidgetOptions>();
	return PrintOptions;
}

URestoreWidgetOptions* UWidgetPrinter::CreateDefaultRestoreOptions() const
{
	auto* RestoreOptions = NewObject<URestoreWidgetOptions>();
	return RestoreOptions;
}

TSharedRef<GraphPrinter::IInnerPrinter> UWidgetPrinter::CreateInnerPrinter() const
{
	return MakeShared<GraphPrinter::FWidgetPrinter>(
		GetPrintOptions(),
		GetRestoreOptions()
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

void UWidgetPrinter::CleanupPrintOptions()
{
	CachedPrintOptions = nullptr;
}

void UWidgetPrinter::CleanupRestoreOptions()
{
	CachedRestoreOptions = nullptr;
}
