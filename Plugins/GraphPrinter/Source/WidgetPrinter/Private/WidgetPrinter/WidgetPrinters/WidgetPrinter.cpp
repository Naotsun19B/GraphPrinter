// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Types/OneWayBool.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "Slate/WidgetRenderer.h"
#include "Engine/TextureRenderTarget2D.h"

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
#include "ClipboardImageExtension/HAL/ClipboardImageExtension.h"
#endif

#define LOCTEXT_NAMESPACE "WidgetPrinte"

void UWidgetPrinter::PrintWidget(UPrintWidgetOptions* Options)
{
	CachedPrintOptions = Options;
	PrintWidget();
}

bool UWidgetPrinter::CanPrintWidget(UPrintWidgetOptions* Options)
{
	CachedPrintOptions = Options;
	return CanPrintWidget();
}

void UWidgetPrinter::RestoreWidget(URestoreWidgetOptions* Options)
{
	CachedRestoreOptions = Options;
	RestoreWidget();
}

bool UWidgetPrinter::CanRestoreWidget(URestoreWidgetOptions* Options)
{
	CachedRestoreOptions = Options;
	return CanRestoreWidget();
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

	return INDEX_NONE;
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

void UWidgetPrinter::PrintWidget()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	Widget = FindTargetWidget(PrintOptions->TargetWidget);

	// If don't have an active graph editor, end here.
	if (!Widget.IsValid())
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotFoundGraphEditorError", "The graph editor isn't currently open."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	// If the restore function cannot be used, the node information will not be embedded.
	if (auto* RestoreOptions = NewObject<URestoreWidgetOptions>())
	{
		RestoreOptions->TargetWidget = PrintOptions->TargetWidget;
		if (!CanRestoreWidget())
		{
			PrintOptions->bIsIncludeNodeInfoInImageFile = false;
		}
	}

	PreCalculateDrawSize();

	if (!CalculateDrawSize(WidgetPrinterParams.DrawSize))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotSelectedError", "No node is selected."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	PreDrawWidget();

	const bool bIsPrintableSize = IsPrintableSize();

	// Draw the graph editor on the render target.
	if (bIsPrintableSize)
	{
		WidgetPrinterParams.RenderTarget = DrawWidgetToRenderTarget();
	}

	PostDrawWidget();

	if (!bIsPrintableSize)
	{
		const FText& Message = FText::FromString(FString::Printf(
			TEXT("%s / %s\nThe drawing range is too wide.\nIf necessary, change the maximum size from the editor settings."),
			*WidgetPrinterParams.DrawSize.ToString(), *PrintOptions->MaxImageSize.ToString()
		));
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			Message,
			GraphPrinter::FGraphPrinterUtils::CS_Fail,
			6.f
		);
		return;
	}

	if (!IsValid(WidgetPrinterParams.RenderTarget))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("DrawError", "Failed to draw to render target."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	PrepareCopyToClipboard();
#endif

	// Create output options and file path and output as image file.
	WidgetPrinterParams.Filename = CreateFilename();

	// Bind the event when the operation is completed.
	PrintOptions->ImageWriteOptions.NativeOnComplete = [=](bool bIsSucceeded)
	{
		OnExportRenderTargetFinished(bIsSucceeded);
	};

	// Export the render target in the specified file format.
	ExportRenderTargetToImageFile();
}

bool UWidgetPrinter::CanPrintWidget()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	if (ShouldAlwaysPrintAll() && PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::Selected)
	{
		return false;
	}

	const TSharedPtr<SWidget> FoundWidget = FindTargetWidget(PrintOptions->TargetWidget);
	return FoundWidget.IsValid();
}

void UWidgetPrinter::RestoreWidget()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	USE_RESTORE_OPTIONS_IN_THIS_FUNCTION();
	
	Widget = FindTargetWidget(RestoreOptions->TargetWidget);

	// If don't have an active graph editor, end here.
	if (!Widget.IsValid())
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("NotFoundGraphEditorError", "The graph editor isn't currently open."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	// Launch the file browser and select the png file.
	if (RestoreOptions->HasValidSourceImageFilePath())
	{
		WidgetPrinterParams.Filename = RestoreOptions->GetSourceImageFilePath();
	}
	else
	{
		TArray<FString> Filenames;
		if (!GraphPrinter::FGraphPrinterUtils::OpenFileDialog(
			Filenames,
			RestoreOptions->DialogTitle,
			RestoreOptions->DefaultPath,
			RestoreOptions->DefaultFile,
			RestoreOptions->FileTypes,
			false
		))
		{
			return;
		}

		if (!Filenames.IsValidIndex(0))
		{
			return;
		}

		WidgetPrinterParams.Filename = FPaths::ConvertRelativePathToFull(Filenames[0]);
	}

	// Restore node from png file.
	if (RestoreWidgetFromTextChunk())
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("SucceededRestore", "Restore nodes from"),
			GraphPrinter::FGraphPrinterUtils::CS_Success, 5.f,
			TArray<GraphPrinter::FNotificationInteraction>{
				GraphPrinter::FNotificationInteraction(
					FText::FromString(WidgetPrinterParams.Filename),
					FSimpleDelegate::CreateLambda([=]()
					{
						GraphPrinter::FGraphPrinterUtils::OpenFolderWithExplorer(WidgetPrinterParams.Filename);
					})
				)
			}
		);
	}
	else
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("FailedRestoreError", "Failed restore nodes."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
	}
#endif
}

bool UWidgetPrinter::CanRestoreWidget()
{
	return false;
}

bool UWidgetPrinter::CalculateDrawSize(FVector2D& DrawSize)
{
	const FGeometry& Geometry =
#if BEFORE_UE_4_23
		Widget->GetCachedGeometry();
#else
		Widget->GetTickSpaceGeometry();
#endif

	DrawSize = Geometry.GetAbsoluteSize();

	return true;
}

bool UWidgetPrinter::IsPrintableSize() const
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	// Check draw size.
	if (PrintOptions->MaxImageSize > FVector2D::ZeroVector)
	{
		if (WidgetPrinterParams.DrawSize.X > PrintOptions->MaxImageSize.X ||
			WidgetPrinterParams.DrawSize.Y > PrintOptions->MaxImageSize.Y)
		{
			return false;
		}
	}

	return true;
}

UTextureRenderTarget2D* UWidgetPrinter::DrawWidgetToRenderTarget()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(PrintOptions->bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(
		WidgetPrinterParams.DrawSize,
		PrintOptions->FilteringMode,
		PrintOptions->bUseGamma
	);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}
	if (PrintOptions->bUseGamma)
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
			PrintOptions->RenderingScale,
			WidgetPrinterParams.DrawSize,
			0.f
		);
		FlushRenderingCommands();
	}

	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

void UWidgetPrinter::PrepareCopyToClipboard()
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	// When copying to the clipboard, set the image format so that it can be copied.
	if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::Clipboard)
	{
		PrintOptions->ImageWriteOptions.Format = ClipboardImageExtension::FClipboardImageExtension::GetCopyableImageFormat();
	}
#endif
}

FString UWidgetPrinter::CreateFilename() const
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(PrintOptions->OutputDirectoryPath, GetWidgetTitle())
	);
	const FString& Extension = GraphPrinter::FWidgetPrinterUtils::GetImageFileExtension(PrintOptions->ImageWriteOptions.Format);

	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			ValidatePathErrorText,
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return FString();
	}

	// If the file cannot be overwritten, add a number after the file name.
	if (!PrintOptions->ImageWriteOptions.bOverwriteFile)
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

void UWidgetPrinter::ExportRenderTargetToImageFile()
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	if (!IsValid(WidgetPrinterParams.RenderTarget))
	{
		return;
	}
			
	UImageWriteBlueprintLibrary::ExportToDisk(
		WidgetPrinterParams.RenderTarget,
		WidgetPrinterParams.Filename,
		PrintOptions->ImageWriteOptions
	);

	// As a symptomatic treatment for the problem that the first image output after startup is whitish,
	// the first output is re-output as many times as NumberOfRedrawsWhenFirstTime.
	if (IsFirstOutput.GetValue())
	{
		for (int32 Count = 0; Count < NumberOfReOutputWhenFirstTime; Count++)
		{
			FImageWriteOptions ImageWriteOptions = PrintOptions->ImageWriteOptions;
			ImageWriteOptions.bOverwriteFile = true;
			ImageWriteOptions.NativeOnComplete = nullptr;
			UImageWriteBlueprintLibrary::ExportToDisk(
				WidgetPrinterParams.RenderTarget,
				WidgetPrinterParams.Filename,
				ImageWriteOptions
			);
			IsFirstOutput.Switch();
		}	
	}
}

void UWidgetPrinter::OnExportRenderTargetFinished(bool bIsSucceeded)
{
	USE_PRINT_OPTIONS_IN_THIS_FUNCTION();
	
	if (!bIsSucceeded)
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("FailedOutputError", "Failed capture GraphEditor."),
			GraphPrinter::FGraphPrinterUtils::CS_Fail
		);
		return;
	}

	if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::ImageFile)
	{
		GraphPrinter::FGraphPrinterUtils::ShowNotification(
			LOCTEXT("SucceededOutput", "GraphEditor capture saved as"),
			GraphPrinter::FGraphPrinterUtils::CS_Success, 5.f,
			TArray<GraphPrinter::FNotificationInteraction>{
				GraphPrinter::FNotificationInteraction(
					FText::FromString(WidgetPrinterParams.Filename),
					FSimpleDelegate::CreateLambda([=]()
					{
						GraphPrinter::FGraphPrinterUtils::OpenFolderWithExplorer(WidgetPrinterParams.Filename);
					})
				)
			}
		);

#ifdef WITH_TEXT_CHUNK_HELPER
		// Embed node information in the output png image.
		// When copying to the clipboard, the process is skipped.
		if (PrintOptions->bIsIncludeNodeInfoInImageFile &&
			PrintOptions->ImageWriteOptions.Format == EDesiredImageFormat::PNG)
		{
			if (!WriteWidgetInfoToTextChunk())
			{
				GraphPrinter::FGraphPrinterUtils::ShowNotification(
					LOCTEXT("FailedEmbedNodeInfoError", "Failed to write node information to png file."),
					GraphPrinter::FGraphPrinterUtils::CS_Fail
				);
			}
		}
#endif
	}
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	else if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::Clipboard)
	{
		if (CopyImageFileToClipboard())
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("SucceededClipboardCopy", "Succeeded to copy image to clipboard."),
				GraphPrinter::FGraphPrinterUtils::CS_Success
			);
		}
		else
		{
			GraphPrinter::FGraphPrinterUtils::ShowNotification(
				LOCTEXT("FailedClipboardCopy", "Failed to copy image to clipboard."),
				GraphPrinter::FGraphPrinterUtils::CS_Fail
			);
		}

		IFileManager::Get().Delete(*WidgetPrinterParams.Filename, false, true);
	}
#endif
}

bool UWidgetPrinter::CopyImageFileToClipboard()
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	return ClipboardImageExtension::FClipboardImageExtension::ClipboardCopy(WidgetPrinterParams.Filename);
#else
	return false;
#endif
}

bool UWidgetPrinter::WriteWidgetInfoToTextChunk()
{
	return false;
}

bool UWidgetPrinter::RestoreWidgetFromTextChunk()
{
	return false;
}

bool UWidgetPrinter::ShouldAlwaysPrintAll() const
{
	return true;
}

UPrintWidgetOptions* UWidgetPrinter::GetPrintOptions() const
{
	return CachedPrintOptions;
}

URestoreWidgetOptions* UWidgetPrinter::GetRestoreOptions() const
{
	return CachedRestoreOptions;
}

GraphPrinter::FOneWayBool UWidgetPrinter::IsFirstOutput = true;

#undef LOCTEXT_NAMESPACE
