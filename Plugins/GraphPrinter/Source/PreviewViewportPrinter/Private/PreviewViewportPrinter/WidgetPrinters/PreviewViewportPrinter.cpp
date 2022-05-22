// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "PreviewViewportPrinter/WidgetPrinters/InnerPreviewViewportPrinter.h"

int32 UPreviewViewportPrinter::GetPriority() const
{
	return PreviewViewportPrinterPriority;
}

TSharedRef<GraphPrinter::IInnerPrinter> UPreviewViewportPrinter::CreatePrintModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FPreviewViewportPrinter>(
		GetPrintOptions(),
		OnPrinterProcessingFinished
	);
}

TSharedRef<GraphPrinter::IInnerPrinter> UPreviewViewportPrinter::CreateRestoreModeInnerPrinter(const FSimpleDelegate& OnPrinterProcessingFinished) const
{
	return MakeShared<GraphPrinter::FPreviewViewportPrinter>(
		GetRestoreOptions(),
		OnPrinterProcessingFinished
	);
}

UTextureRenderTarget2D* UPreviewViewportPrinter::GetRenderedPreviewViewport(UPrintWidgetOptions* Options)
{
	auto* DefaultObject = GetDefault<UPreviewViewportPrinter>();
	check(IsValid(DefaultObject));

	UTextureRenderTarget2D* RenderedPreviewViewport = nullptr;
	TSharedRef<GraphPrinter::IInnerPrinter> InnerPrinter = MakeShared<GraphPrinter::FPreviewViewportPrinter>(
		DefaultObject->GetPrintOptions(),
		GraphPrinter::FPreviewViewportPrinter::FOnPreviewViewportRendered::CreateLambda(
			[&](TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget)
			{
				RenderedPreviewViewport = RenderTarget.Get();
			}
		)
	);

	return RenderedPreviewViewport;
}
