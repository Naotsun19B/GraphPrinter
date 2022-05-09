// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/WidgetPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Slate/WidgetRenderer.h"
#include "Engine/TextureRenderTarget2D.h"

UTextureRenderTarget2D* UWidgetPrinter::DrawWidgetToRenderTarget(
	const TSharedPtr<SWidget>& Widget,
	const FVector2D& DrawSize,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(Options.bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Options.FilteringMode, Options.bUseGamma);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}
	if (Options.bUseGamma)
	{
		RenderTarget->bForceLinearGamma = true;
		RenderTarget->UpdateResourceImmediate(true);
	}

	// Since the drawing result may be corrupted the first time, draw multiple times.
	for (int32 Count = 0; Count < DrawTimes; Count++)
	{
		WidgetRenderer->DrawWidget(
			RenderTarget,
			Widget.ToSharedRef(),
			Options.RenderingScale,
			DrawSize,
			0.f
		);
		FlushRenderingCommands();
	}

	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

GraphPrinter::FOneWayBool UWidgetPrinter::IsFirstOutput = true;
