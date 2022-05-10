// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/WidgetPrinter.h"
#include "GraphPrinter/Utilities/WidgetPrinterUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "Slate/WidgetRenderer.h"
#include "Engine/TextureRenderTarget2D.h"

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
#include "ClipboardImageExtension/HAL/ClipboardImageExtension.h"
#endif

bool UWidgetPrinter::IsPrintableSize(
	const TSharedPtr<SWidget>& Widget,
	const FVector2D& DrawSize,
	const GraphPrinter::FPrintWidgetOptions& Options
) const
{
	// Check draw size.
	if (Options.MaxImageSize > FVector2D::ZeroVector)
	{
		if (DrawSize.X > Options.MaxImageSize.X || DrawSize.Y > Options.MaxImageSize.Y)
		{
			return false;
		}
	}

	return true;
}

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

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
void UWidgetPrinter::PrepareCopyToClipboard(GraphPrinter::FPrintWidgetOptions& Options)
{
	// When copying to the clipboard, set the image format so that it can be copied.
	if (Options.ExportMethod == GraphPrinter::EExportMethod::Clipboard)
	{
		Options.ImageWriteOptions.Format = ClipboardImageExtension::FClipboardImageExtension::GetCopyableImageFormat();
	}
}
#endif

FString UWidgetPrinter::CreateFilename(
	const TSharedPtr<SWidget>& Widget,
	const GraphPrinter::FPrintWidgetOptions& Options
) const
{
	FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, GetWidgetTitle(Widget))
	);
	const FString& Extension = GraphPrinter::FWidgetPrinterUtils::GetImageFileExtension(Options.ImageWriteOptions.Format);

	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(ValidatePathErrorText, GraphPrinter::FGraphPrinterUtils::CS_Fail);
		return FString();
	}

	// If the file cannot be overwritten, add a number after the file name.
	if (!Options.ImageWriteOptions.bOverwriteFile)
	{
		if (IFileManager::Get().FileExists(*FString(Filename + Extension)))
		{
			auto CombineFilenameAndIndex = [](const FString& Filename, int32 Index) -> FString
			{
				return FString::Printf(TEXT("%s_%d"), *Filename, Index);
			};

			int32 Index = 0;
			while (Index < TNumericLimits<int32>().Max())
			{
				const FString& FilenameWithExtension = CombineFilenameAndIndex(Filename, Index) + Extension;
				if (!IFileManager::Get().FileExists(*FilenameWithExtension))
				{
					break;
				}
				Index++;
			}
			Filename = CombineFilenameAndIndex(Filename, Index);
		}
	}

	return (Filename + Extension);
}

FString UWidgetPrinter::GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const
{
	return {};
}

void UWidgetPrinter::ExportRenderTargetToImageFile(
	UTextureRenderTarget2D* RenderTarget,
	const FString& Filename,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, Options.ImageWriteOptions);

	// As a symptomatic treatment for the problem that the first image output after startup is whitish,
	// the first output is re-output as many times as NumberOfRedrawsWhenFirstTime.
	if (IsFirstOutput.GetValue())
	{
		for (int32 Count = 0; Count < NumberOfReOutputWhenFirstTime; Count++)
		{
			FImageWriteOptions ImageWriteOptions = Options.ImageWriteOptions;
			ImageWriteOptions.bOverwriteFile = true;
			ImageWriteOptions.NativeOnComplete = nullptr;
			UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, ImageWriteOptions);
			IsFirstOutput.Switch();
		}	
	}
}

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
bool UWidgetPrinter::CopyImageFileToClipboard(
	const FString& Filename,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	return ClipboardImageExtension::FClipboardImageExtension::ClipboardCopy(Filename);
}
#endif

GraphPrinter::FOneWayBool UWidgetPrinter::IsFirstOutput = true;
