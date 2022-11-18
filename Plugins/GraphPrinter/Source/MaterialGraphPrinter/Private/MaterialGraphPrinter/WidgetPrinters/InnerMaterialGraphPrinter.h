// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"
#include "MaterialGraphPrinter/Types/PrintMaterialGraphOptions.h"
#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "MaterialGraph/MaterialGraph.h"
#include "SGraphEditorImpl.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"

#if BEFORE_UE_4_27
#include "MaterialGraph/MaterialGraphNode_Root.h"
#endif

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class MATERIALGRAPHPRINTER_API FMaterialGraphPrinter
		: public TGraphPrinter<UPrintMaterialGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintMaterialGraphOptions, URestoreWidgetOptions>;

	public:
		// Constructor.
		FMaterialGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InPrintOptions, InOnPrinterProcessingFinished)
		{
		}
		FMaterialGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
			: Super(InRestoreOptions, InOnPrinterProcessingFinished)
		{
		}

		// TInnerWidgetPrinter interface.
		virtual bool CanPrintWidget() const override
		{
			if (Super::CanPrintWidget())
			{
				const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
				return IsValid(Cast<UMaterialGraph>(GraphEditor->GetCurrentGraph()));
			}

			return false;
		}
		virtual UTextureRenderTarget2D* DrawWidgetToRenderTarget() override
		{
			UTextureRenderTarget2D* RenderedGraph = Super::DrawWidgetToRenderTarget();
			if (!IsValid(RenderedGraph))
			{
				return nullptr;
			}

			if (PrintOptions->MaterialGraphExportMethod == EMaterialGraphExportMethod::GraphOnly)
			{
				return RenderedGraph;
			}

			auto* ToRenderTarget = PrintOptions->Duplicate(UPrintWidgetOptions::StaticClass());
			ToRenderTarget->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			ToRenderTarget->ExportMethod = UPrintWidgetOptions::EExportMethod::RenderTarget;
			ToRenderTarget->SearchTarget = FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(Widget);;
			const UWidgetPrinter::FRenderingResult RenderingResult = GetRenderingResult<UPreviewViewportPrinter>(ToRenderTarget);
			if (!RenderingResult.IsValid())
			{
				return nullptr;
			}

			if (PrintOptions->MaterialGraphExportMethod == EMaterialGraphExportMethod::PreviewAndGraphSeparately)
			{
				ExportRenderTargetToImageFileInternal(
					RenderingResult.RenderTarget.Get(),
					RenderingResult.Filename,
					PrintOptions->ImageWriteOptions
				);
				
				return RenderedGraph;
			}

			// Re-render the widget that concatenates the two render targets.
			const TSharedPtr<FSlateBrush> PreviewViewportBrush = MakeShared<FSlateBrush>();
			PreviewViewportBrush->SetResourceObject(RenderingResult.RenderTarget.Get());
			const TSharedPtr<FSlateBrush> GraphBrush = MakeShared<FSlateBrush>();
			GraphBrush->SetResourceObject(RenderedGraph);

			const TSharedRef<SWidget> CombinedWidget = SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RenderingResult.RenderTarget->SizeX)
					.HeightOverride(RenderingResult.RenderTarget->SizeY)
					[
						SNew(SImage)
						.Image(PreviewViewportBrush.Get())
					]
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RenderedGraph->SizeX)
					.HeightOverride(RenderedGraph->SizeY)
					[
						SNew(SImage)
						.Image(GraphBrush.Get())
					]
				];

#if BEFORE_UE_4_27
			CombinedWidget->InvalidatePrepass();
#else
			CombinedWidget->MarkPrepassAsDirty();
#endif

			const FVector2D CombinedSize(
				RenderingResult.RenderTarget->SizeX + RenderedGraph->SizeX,
				FMath::Max(RenderingResult.RenderTarget->SizeY, RenderedGraph->SizeY)
			);
			
			return DrawWidgetToRenderTargetInternal(
				CombinedWidget,
				CombinedSize,
				PrintOptions->FilteringMode,
				false, // If draw with gamma twice, it will be too bright, so gamma is not used here.
				PrintOptions->RenderingScale
			);
		}
		virtual FString GetWidgetTitle() override
		{
			if (const auto* MaterialGraph = Cast<UMaterialGraph>(Widget->GetCurrentGraph()))
			{
#if BEFORE_UE_4_27
				if (const UMaterial* Material = MaterialGraph->Material)
#else
				if (const UMaterial* Material = MaterialGraph->Material.Get())
#endif
				{
					FString GraphName = TEXT("MaterialGraph");
#if BEFORE_UE_4_27
					if (IsValid(MaterialGraph->RootNode))
#else
					if (MaterialGraph->RootNode)
#endif
					{
						GraphName = MaterialGraph->GetName();
					}
				
					return FString::Printf(TEXT("%s-%s"), *Material->GetName(), *GraphName);
				}
			}

			return TEXT("InvalidMaterialEditor");
		}
		// End of TInnerWidgetPrinter interface.
	};
}
