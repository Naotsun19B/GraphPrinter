// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetPrinter/WidgetPrinters/WidgetPrinter.h"
#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "WidgetPrinter/Types/OneWayBool.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "Slate/WidgetRenderer.h"
#include "UObject/StrongObjectPtr.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "EdGraph/EdGraph.h"

#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
#include "ClipboardImageExtension/HAL/ClipboardImageExtension.h"
#endif

#define LOCTEXT_NAMESPACE "InnerPrinter"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore widgets.
	 */
	class WIDGETPRINTER_API IInnerWidgetPrinter
	{
	public:
		// Defines the event when receiving the drawing result without outputting the render target.
		DECLARE_DELEGATE_OneParam(FOnRendered, UWidgetPrinter::FRenderingResult /* RenderingResult */);
		
	public:
		// Destructor.
		virtual ~IInnerWidgetPrinter() = default;
		
		// Draw and export the widget with arguments.
		virtual void PrintWidget() = 0;
	
		// Returns whether the target widget can be printed.
		virtual bool CanPrintWidget() const = 0;
	
		// Restore the state of the widget from the image file.
		virtual void RestoreWidget() = 0;

		// Returns whether the target widget can be restored.
		virtual bool CanRestoreWidget() const = 0;

		// Sets event when receiving the drawing result without outputting the render target.
		void SetOnRendered(const FOnRendered& InOnRendered);

	protected:
		// Draw the widget on the render target.
		static UTextureRenderTarget2D* DrawWidgetToRenderTargetInternal(
			const TSharedRef<SWidget>& Widget,
			const FVector2D& DrawSize,
			const TextureFilter FilteringMode,
			const bool bUseGamma,
			const float RenderingScale
		);

		// Export the render target that draws the graph editor to image file.
		static void ExportRenderTargetToImageFileInternal(
			UTextureRenderTarget2D* RenderTarget,
			const FString& Filename,
			const FImageWriteOptions& ImageWriteOptions
		);
		
	protected:
		// Event when receiving the drawing result without outputting the render target.
		FOnRendered OnRendered;
		
		// Number of attempts to draw the widget on the render target.
		// The drawing result may be corrupted once.
		// Probably if draw twice, the drawing result will not be corrupted.
		static constexpr int32 DrawTimes = 2;

		// The number of times to re-output as a countermeasure against the whitish image
		// that is output for the first time after starting the engine.
		static constexpr int32 NumberOfReOutputWhenFirstTime = 2;
		static FOneWayBool IsFirstOutput;
	};

	/**
	 * A class template that implements the actual processing of the inner class.
	 */
	template<class TWidget, class TPrintOptions, class TRestoreOptions>
	class TInnerWidgetPrinter : public IInnerWidgetPrinter
	{
	public:
		static_assert(TIsDerivedFrom<TWidget, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
		static_assert(TIsDerivedFrom<TPrintOptions, UPrintWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UPrintWidgetOptions.");
		static_assert(TIsDerivedFrom<TRestoreOptions, URestoreWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of URestoreWidgetOptions.");
	
	public:
		// Constructor.
		explicit TInnerWidgetPrinter(
			UPrintWidgetOptions* InPrintOptions,
			const FSimpleDelegate& InOnPrinterProcessingFinished
		)
			: OnPrinterProcessingFinished(InOnPrinterProcessingFinished)
			, RestoreOptions(nullptr)
		{
			PrintOptions = Cast<TPrintOptions>(InPrintOptions);
			if (!IsValid(PrintOptions))
			{
				PrintOptions = InPrintOptions->Duplicate<TPrintOptions>();
			}
		}
		explicit TInnerWidgetPrinter(
			URestoreWidgetOptions* InRestoreOptions,
			const FSimpleDelegate& InOnPrinterProcessingFinished
		)
			: OnPrinterProcessingFinished(InOnPrinterProcessingFinished)
			, PrintOptions(nullptr)
		{
			RestoreOptions = Cast<TRestoreOptions>(InRestoreOptions);
			if (!IsValid(RestoreOptions))
			{
				RestoreOptions = InRestoreOptions->Duplicate<TRestoreOptions>();
			}
		}
		
		// IInnerWidgetPrinter interface.
		virtual void PrintWidget() override
		{
			if (!IsValid(PrintOptions))
			{
				return;
			}
			
			Widget = FindTargetWidget(PrintOptions->SearchTarget);
			if (!Widget.IsValid())
			{
				return;
			}

			// If the restore function cannot be used, the widget information will not be embedded.
			RestoreOptions = NewObject<URestoreWidgetOptions>();
			if (IsValid(RestoreOptions))
			{
				RestoreOptions->SearchTarget = PrintOptions->SearchTarget;
				if (!CanRestoreWidget())
				{
					PrintOptions->bIsIncludeNodeInfoInImageFile = false;
				}
			}

			PreCalculateDrawSize();

			if (!CalculateDrawSize(WidgetPrinterParams.DrawSize))
			{
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("NotSelectedError", "No node is selected."),
					FGraphPrinterUtils::CS_Fail
				);
				return;
			}

			// Adjust the draw size according to the rendering scale.
			WidgetPrinterParams.DrawSize *= PrintOptions->RenderingScale;

			PreDrawWidget();

			const bool bIsPrintableSize = IsPrintableSize();

			// Draw the widget on the render target.
			if (bIsPrintableSize)
			{
				WidgetPrinterParams.RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(DrawWidgetToRenderTarget());
			}

			PostDrawWidget();

			if (!bIsPrintableSize)
			{
				const FText& Message = FText::Format(
					LOCTEXT("TooLargeSize", "{DrawSize} / {MaxImageSize}\nThe drawing range is too wide.\nIf necessary, change the maximum size from the editor preferences."),
					FText::FromString(WidgetPrinterParams.DrawSize.ToString()),
					FText::FromString(PrintOptions->MaxImageSize.ToString())
				);
				FGraphPrinterUtils::ShowNotification(
					Message,
					FGraphPrinterUtils::CS_Fail,
					6.f,
					TArray<FNotificationInteraction>{
						FNotificationInteraction(
							LOCTEXT("OpenSettingsNavigation", "Jump to the editor preferences."),
							FSimpleDelegate::CreateLambda(
								[]()
								{
									OpenSettings<UWidgetPrinterSettings>();
								}
							)
						)
					}
				);
				return;
			}

			if (!WidgetPrinterParams.RenderTarget.IsValid())
			{
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("DrawError", "Failed to draw to render target."),
					FGraphPrinterUtils::CS_Fail
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
		virtual bool CanPrintWidget() const override
		{
			if (!IsValid(PrintOptions))
			{
				return false;
			}
			
			if (ShouldAlwaysPrintAll() && PrintOptions->PrintScope == UPrintWidgetOptions::EPrintScope::Selected)
			{
				return false;
			}

			const TSharedPtr<TWidget> FoundWidget = FindTargetWidget(PrintOptions->SearchTarget);
			return FoundWidget.IsValid();
		}
		virtual void RestoreWidget() override
		{
#ifdef WITH_TEXT_CHUNK_HELPER
			if (!IsValid(RestoreOptions))
			{
				return;
			}
			
			Widget = FindTargetWidget(RestoreOptions->SearchTarget);
			if (!Widget.IsValid())
			{
				return;
			}

			// Launch the file browser and select the iamge file.
			if (RestoreOptions->HasValidSourceImageFilePath())
			{
				WidgetPrinterParams.Filename = RestoreOptions->GetSourceImageFilePath();
			}
			else
			{
				TArray<FString> Filenames;
				if (!FGraphPrinterUtils::OpenFileDialog(
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
			
			if (RestoreWidgetFromTextChunk())
			{
				const FString Filename = WidgetPrinterParams.Filename;
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("SucceededRestore", "Restore widget from"),
					FGraphPrinterUtils::CS_Success, 5.f,
					TArray<FNotificationInteraction>{
						FNotificationInteraction(
							FText::FromString(Filename),
							FSimpleDelegate::CreateLambda([Filename]()
							{
								FGraphPrinterUtils::OpenFolderWithExplorer(Filename);
							})
						)
					}
				);
			}
			else
			{
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("FailedRestoreError", "Failed restore widget."),
					FGraphPrinterUtils::CS_Fail
				);
			}
#endif
		}
		virtual bool CanRestoreWidget() const override
		{
			return false;
		}
		// End of IInnerWidgetPrinter interface.

	protected:
		// Returns the widget to print or restore.
		virtual TSharedPtr<TWidget> FindTargetWidget(const TSharedPtr<SWidget>& SearchTarget) const
		{
			return nullptr;
		}

		// Called before calculating the size of the widget to draw.
		virtual void PreCalculateDrawSize() {}

		// Calculate the size to use when drawing the widget.
		virtual bool CalculateDrawSize(FVector2D& DrawSize)
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

		// Performs processing before draw the widget.
		virtual void PreDrawWidget() {}
		
		// Returns whether the widget size is drawable.
		virtual bool IsPrintableSize() const
		{
			// Check draw size.
#if BEFORE_UE_5_00
			if (PrintOptions->MaxImageSize > FVector2D::ZeroVector)
#else
			if (PrintOptions->MaxImageSize.ComponentwiseAllGreaterThan(FVector2D::ZeroVector))
#endif
			{
				if (WidgetPrinterParams.DrawSize.X > PrintOptions->MaxImageSize.X ||
					WidgetPrinterParams.DrawSize.Y > PrintOptions->MaxImageSize.Y)
				{
					return false;
				}
			}

			return true;
		}
	
		// Draw the widget on the render target.
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget()
		{
			return DrawWidgetToRenderTargetInternal(
				Widget.ToSharedRef(),
				WidgetPrinterParams.DrawSize,
				PrintOptions->FilteringMode,
				PrintOptions->bUseGamma,
				PrintOptions->RenderingScale
			);
		}

		// Performs processing after draw the widget.
		virtual void PostDrawWidget() {}

		// Prepare for copying to the clipboard.
		virtual void PrepareCopyToClipboard()
		{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
			// When copying to the clipboard, set the image format so that it can be copied.
			if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::Clipboard)
			{
				PrintOptions->ImageWriteOptions.Format = ClipboardImageExtension::FClipboardImageExtension::GetCopyableImageFormat();
			}
#endif
		}

		// Create a file path from options.
		virtual FString CreateFilename()
		{
			FString Filename = FPaths::ConvertRelativePathToFull(
				FPaths::Combine(PrintOptions->OutputDirectoryPath, GetWidgetTitle())
			);
			const FString& Extension = FGraphPrinterUtils::GetImageFileExtension(PrintOptions->ImageWriteOptions.Format);

			FText ValidatePathErrorText;
			if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
			{
				FGraphPrinterUtils::ShowNotification(ValidatePathErrorText, FGraphPrinterUtils::CS_Fail);
				return {};
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

		// Returns the title of the widget.
		virtual FString GetWidgetTitle()
		{
			return {};
		}

		// Export the render target that draws the widget to image file.
		virtual void ExportRenderTargetToImageFile()
		{
			if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::RenderTarget)
			{
				UWidgetPrinter::FRenderingResult RenderingResult;
				RenderingResult.DrawSize = WidgetPrinterParams.DrawSize;
				RenderingResult.RenderTarget = WidgetPrinterParams.RenderTarget;
				RenderingResult.Filename = WidgetPrinterParams.Filename;
				OnRendered.ExecuteIfBound(RenderingResult);
				OnPrinterProcessingFinished.ExecuteIfBound();
			}
			else
			{
				ExportRenderTargetToImageFileInternal(
					WidgetPrinterParams.RenderTarget.Get(),
					WidgetPrinterParams.Filename,
					PrintOptions->ImageWriteOptions
				);
			}
		}

		// Called when the image file export process is complete.
		virtual void OnExportRenderTargetFinished(bool bIsSucceeded)
		{
			if (!bIsSucceeded)
			{
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("FailedOutputError", "Failed capture widget."),
					FGraphPrinterUtils::CS_Fail
				);
				return;
			}

			if (PrintOptions->ExportMethod == UPrintWidgetOptions::EExportMethod::ImageFile)
			{
				const FString Filename = WidgetPrinterParams.Filename;
				FGraphPrinterUtils::ShowNotification(
					LOCTEXT("SucceededOutput", "Capture saved as"),
					FGraphPrinterUtils::CS_Success, 5.f,
					TArray<FNotificationInteraction>{
						FNotificationInteraction(
							FText::FromString(Filename),
							FSimpleDelegate::CreateLambda([Filename]()
							{
								FGraphPrinterUtils::OpenFolderWithExplorer(Filename);
							})
						)
					}
				);

#ifdef WITH_TEXT_CHUNK_HELPER
				// Embed information of widget in the output image file.
				// When copying to the clipboard, the process is skipped.
				if (PrintOptions->bIsIncludeNodeInfoInImageFile &&
					PrintOptions->ImageWriteOptions.Format == EDesiredImageFormat::PNG)
				{
					if (!WriteWidgetInfoToTextChunk())
					{
						FGraphPrinterUtils::ShowNotification(
							LOCTEXT("FailedEmbedWidgetInfoError", "Failed to write widget information to image file."),
							FGraphPrinterUtils::CS_Fail
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
					FGraphPrinterUtils::ShowNotification(
						LOCTEXT("SucceededClipboardCopy", "Succeeded to copy image to clipboard."),
						FGraphPrinterUtils::CS_Success
					);
				}
				else
				{
					FGraphPrinterUtils::ShowNotification(
						LOCTEXT("FailedClipboardCopy", "Failed to copy image to clipboard."),
						FGraphPrinterUtils::CS_Fail
					);
				}

				IFileManager::Get().Delete(*WidgetPrinterParams.Filename, false, true);
			}
#endif

			OnPrinterProcessingFinished.ExecuteIfBound();
		}

		// Copy the image file that draws the widget to clipboard.
		virtual bool CopyImageFileToClipboard()
		{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
			return ClipboardImageExtension::FClipboardImageExtension::ClipboardCopy(WidgetPrinterParams.Filename);
#else
			return false;
#endif
		}

		// Write the drawn widget information to the text chunk.
		virtual bool WriteWidgetInfoToTextChunk()
		{
			return false;
		}

		// Read information from the text chunk and restore the widget.
		virtual bool RestoreWidgetFromTextChunk()
		{
			return false;
		}

		// Returns whether to use the optional PrintScope setting.
		virtual bool ShouldAlwaysPrintAll() const
		{
			return true;
		}
		
	protected:
		// An event that notifies the end of processing.
		FSimpleDelegate OnPrinterProcessingFinished;
		
		// Optional class for print processing.
		TPrintOptions* PrintOptions;

		// Optional class for restore processing.
		TRestoreOptions* RestoreOptions;

		// The widget to draw.
		TSharedPtr<TWidget> Widget;
		
		// A group of parameters that must be retained for processing.
		struct FWidgetPrinterParams
		{
			// The size of the image to output.
			FVector2D DrawSize;

			// A render target that holds the drawing results to be output.
			TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget = nullptr;
		
			// The full path of the output file.
			FString Filename;
		};
		FWidgetPrinterParams WidgetPrinterParams;
	};

	class WIDGETPRINTER_API FWidgetPrinter : public TInnerWidgetPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TInnerWidgetPrinter<SWidget, UPrintWidgetOptions, URestoreWidgetOptions>;
		
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

#undef LOCTEXT_NAMESPACE
