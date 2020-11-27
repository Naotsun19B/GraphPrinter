// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterCore.h"
#include "GraphPrinterGlobals.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"
#include "GraphEditor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "ImageUtils.h"

#define LOCTEXT_NAMESPACE "GraphPrinter"

const GraphPrinterCore::TCompletionState GraphPrinterCore::CS_Pending = SNotificationItem::ECompletionState::CS_Pending;
const GraphPrinterCore::TCompletionState GraphPrinterCore::CS_Success = SNotificationItem::ECompletionState::CS_Success;
const GraphPrinterCore::TCompletionState GraphPrinterCore::CS_Fail = SNotificationItem::ECompletionState::CS_Fail;

bool GraphPrinterCore::ShowNotification(const FText NotificationText, TCompletionState CompletionState, const float ExpireDuration)
{
	FNotificationInfo NotificationInfo(NotificationText);
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.ExpireDuration = ExpireDuration;
	NotificationInfo.bUseLargeFont = true;
	//NotificationInfo.Image = &Brush;

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	if (NotificationItem.IsValid())
	{
		auto StateEnum = static_cast<SNotificationItem::ECompletionState>(CompletionState);
		NotificationItem->SetCompletionState(StateEnum);

		return true;
	}
	
	return false;
}

void GraphPrinterCore::CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren)
{
	if (SearchTarget != nullptr)
	{
		if (FChildren* Children = SearchTarget->GetAllChildren())
		{
			for (int32 Index = 0; Index < Children->Num(); Index++)
			{
				TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
				OutChildren.Add(ChildWidget);
				CollectAllChildWidgets(ChildWidget, OutChildren);
			}
		}
	}
}

TSharedPtr<SGraphEditor> GraphPrinterCore::FindGraphEditor(TSharedPtr<SWidget> SearchTarget)
{
	TArray<TSharedPtr<SWidget>> ChildWidgets;
	CollectAllChildWidgets(SearchTarget, ChildWidgets);

	for (const auto& ChildWidget : ChildWidgets)
	{
		TSharedPtr<SGraphEditor> GraphEditor = CAST_SLATE_WIDGET(SGraphEditor, ChildWidget);
		if (GraphEditor.IsValid())
		{
			return GraphEditor;
		}
	}

	return nullptr;
}

UTextureRenderTarget2D* GraphPrinterCore::DrawWidgetToRenderTarget(TSharedPtr<SWidget> WidgetToRender, FVector2D DrawSize, bool bUseGamma, TextureFilter Filter)
{
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, false);
	if (WidgetRenderer == nullptr)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
		return nullptr;
	}

	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGamma);
	if (!IsValid(RenderTarget))
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
		return nullptr;
	}

	WidgetRenderer->DrawWidget(RenderTarget, WidgetToRender.ToSharedRef(), DrawSize, 0.f, false);
	FlushRenderingCommands();
	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

bool GraphPrinterCore::ExportRenderTargetToDisk(UTextureRenderTarget2D* RenderTarget, const FString& Filename, EDesiredImageFormat ImageFormat)
{
	if (!IsValid(RenderTarget))
	{
		return false;
	}
	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
	{
		ShowNotification(ValidatePathErrorText, CS_Fail);
		return false;
	}

	FTextureRenderTargetResource* Resource = RenderTarget->GameThread_GetRenderTargetResource();
	if (Resource == nullptr)
	{
		return false;
	}
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);

	TArray<FColor> OutBMP;
	OutBMP.AddUninitialized(RenderTarget->GetSurfaceWidth() * RenderTarget->GetSurfaceHeight());
	Resource->ReadPixels(OutBMP, ReadPixelFlags);
	for (auto& Pixel : OutBMP)
	{
		Pixel.A = 255;
	}

	FIntPoint ImageSize(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight());
	TArray<uint8> CompressedBitmap;
	FImageUtils::CompressImageArray(ImageSize.X, ImageSize.Y, OutBMP, CompressedBitmap);

	const FString& FilenameWithExtension = FPaths::ConvertRelativePathToFull(FPaths::GetBaseFilename(Filename, false) + GetImageFileExtension(ImageFormat));
	return FFileHelper::SaveArrayToFile(CompressedBitmap, *FilenameWithExtension);
}

bool GraphPrinterCore::CalculateGraphSize(TSharedPtr<SGraphEditor> GraphEditor, FVector2D& GraphSize, bool bSelectedNodeOnly)
{
	GraphSize = FVector2D(1920, 1080);
	return GraphEditor.IsValid();
}

FString GraphPrinterCore::GetImageFileExtension(EDesiredImageFormat ImageFormat)
{
	switch (ImageFormat)
	{
	case EDesiredImageFormat::PNG: return TEXT(".png");
	case EDesiredImageFormat::JPG: return TEXT(".jpg");
	case EDesiredImageFormat::BMP: return TEXT(".bmp");
	case EDesiredImageFormat::EXR: return TEXT(".exr");
	default: break;
	}

	return TEXT("");
}

#undef LOCTEXT_NAMESPACE
