// Copyright 2020-2026 Naotsun. All Rights Reserved.

#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"
#include "Slate/WidgetRenderer.h"
#include "RenderingThread.h"

namespace GraphPrinter
{
	void IInnerWidgetPrinter::SetOnRendered(const FOnRendered& InOnRendered)
	{
		OnRendered = InOnRendered;
	}

	UTextureRenderTarget2D* IInnerWidgetPrinter::DrawWidgetToRenderTargetInternal(
		const TSharedRef<SWidget>& Widget,
		const FVector2D& DrawSize,
		const TextureFilter FilteringMode,
		const bool bUseGamma,
		const float RenderingScale
	)
	{
		FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, false);
		if (WidgetRenderer == nullptr)
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
			return nullptr;
		}

		// Creates the render target manually instead of using FWidgetRenderer::CreateTargetFor
		// to ensure bForceLinearGamma is set BEFORE InitCustomFormat creates the GPU resource.
		// CreateTargetFor creates the resource with bForceLinearGamma=false (via InitCustomFormat)
		// and then the code changes it to true afterwards, but UpdateResourceImmediate only clears
		// the existing resource without recreating it with the new format.
		// This causes the GPU resource to remain in sRGB format while the shader outputs
		// gamma-corrected values, resulting in double gamma correction (whitish appearance).
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
		if (!IsValid(RenderTarget))
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
			return nullptr;
		}
		RenderTarget->Filter = FilteringMode;
		RenderTarget->ClearColor = FLinearColor::Transparent;
		RenderTarget->SRGB = bUseGamma;
		RenderTarget->bForceLinearGamma = bUseGamma;
		RenderTarget->TargetGamma = 1.f;
		RenderTarget->InitCustomFormat(DrawSize.X, DrawSize.Y, PF_B8G8R8A8, bUseGamma);
		RenderTarget->UpdateResourceImmediate(true);
		FlushRenderingCommands();

		WidgetRenderer->DrawWidget(
			RenderTarget,
			Widget,
			RenderingScale,
			DrawSize,
			0.f
		);
		FlushRenderingCommands();

		BeginCleanup(WidgetRenderer);

		return RenderTarget;
	}

	void IInnerWidgetPrinter::ExportRenderTargetToImageFileInternal(
		UTextureRenderTarget2D* RenderTarget,
		const FString& Filename,
		const FImageWriteOptions& ImageWriteOptions
	)
	{
		if (!IsValid(RenderTarget))
		{
			return;
		}

		UImageWriteBlueprintLibrary::ExportToDisk(
			RenderTarget,
			Filename,
			ImageWriteOptions
		);
	}
}
