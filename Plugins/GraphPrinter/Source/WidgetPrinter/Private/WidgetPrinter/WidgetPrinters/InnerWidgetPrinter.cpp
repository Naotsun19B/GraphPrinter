// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/WidgetPrinters/InnerWidgetPrinter.h"

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

		UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(
			DrawSize,
			FilteringMode,
			bUseGamma
		);
		if (!IsValid(RenderTarget))
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
			return nullptr;
		}
		if (bUseGamma)
		{
			RenderTarget->bForceLinearGamma = true;
			RenderTarget->UpdateResourceImmediate(true);
		}

		// Since the drawing result may be corrupted the first time, draw multiple times.
		for (int32 Count = 0; Count < DrawTimes; Count++)
		{
			WidgetRenderer->DrawWidget(
				RenderTarget,
				Widget,
				RenderingScale,
				DrawSize,
				0.f
			);
			FlushRenderingCommands();
		}

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

		// As a symptomatic treatment for the problem that the first image output after startup is whitish,
		// the first output is re-output as many times as NumberOfRedrawsWhenFirstTime.
		if (IsFirstOutput.GetValue())
		{
			for (int32 Count = 0; Count < NumberOfReOutputWhenFirstTime; Count++)
			{
				FImageWriteOptions ForceOverwriteAndQuiet = ImageWriteOptions;
				ForceOverwriteAndQuiet.bOverwriteFile = true;
				ForceOverwriteAndQuiet.NativeOnComplete = nullptr;
				UImageWriteBlueprintLibrary::ExportToDisk(
					RenderTarget,
					Filename,
					ForceOverwriteAndQuiet
				);
				IsFirstOutput.Switch();
			}	
		}
	}

	FOneWayBool IInnerWidgetPrinter::IsFirstOutput = true;
}
