// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"
#include "MaterialGraph/MaterialGraph.h"
#include "SGraphEditorImpl.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"

namespace GraphPrinter
{
	/**
	 * An inner class with the ability to print and restore graph editors.
	 */
	class MATERIALGRAPHPRINTER_API FMaterialGraphPrinter
		: public TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>
	{
	public:
		using Super = TGraphPrinter<UPrintGraphOptions, URestoreWidgetOptions>;

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

		// TInnerPrinter interface.
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

			//if (Options.MaterialGraphExportMethod == EMaterialGraphExportMethod::GraphOnly)
			//{
			//	return RenderedGraph;
			//}

			const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = GraphPrinter::FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(Widget);

			//if (Options.MaterialGraphExportMethod == EMaterialGraphExportMethod::PreviewAndGraphSeparately)
			//{
			//	if (auto* PreviewViewportPrinter = GetMutableDefault<UPreviewViewportPrinter>())
			//	{
			//		GraphPrinter::FPrintWidgetOptions OptionWithTargetWidget = Options;
			//		OptionWithTargetWidget.TargetWidget = StandaloneAssetEditorToolkitHost;
			//		PreviewViewportPrinter->PrintWidget(OptionWithTargetWidget);
			//		return RenderedGraph;
			//	}
			//}

			UTextureRenderTarget2D* RenderedPreviewViewport = UPreviewViewportPrinter::GetRenderedPreviewViewport(PrintOptions);
			if (!IsValid(RenderedPreviewViewport))
			{
				return nullptr;
			}

			// Re-render the widget that concatenates the two render targets.
			const TSharedPtr<FSlateBrush> PreviewViewportBrush = MakeShared<FSlateBrush>();
			PreviewViewportBrush->SetResourceObject(RenderedPreviewViewport);
			const TSharedPtr<FSlateBrush> GraphBrush = MakeShared<FSlateBrush>();
			GraphBrush->SetResourceObject(RenderedGraph);

			const TSharedRef<SWidget> CombinedWidget = SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Top)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(RenderedPreviewViewport->SizeX)
					.HeightOverride(RenderedPreviewViewport->SizeY)
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

			CombinedWidget->MarkPrepassAsDirty();

			const FVector2D CombinedSize(
				RenderedPreviewViewport->SizeX + RenderedGraph->SizeX,
				FMath::Max(RenderedPreviewViewport->SizeY, RenderedGraph->SizeY)
			);
			
			auto* GammaLess = PrintOptions->Duplicate(UPrintWidgetOptions::StaticClass());
			GammaLess->bUseGamma = false;
			
			return DrawWidgetToRenderTargetInternal(
				CombinedWidget,
				CombinedSize,
				PrintOptions->FilteringMode,
				PrintOptions->bUseGamma,
				PrintOptions->RenderingScale
			);
		}
		virtual FString GetWidgetTitle() override
		{
			if (const auto* MaterialGraph = Cast<UMaterialGraph>(Widget->GetCurrentGraph()))
			{
				if (const UMaterial* Material = MaterialGraph->Material.Get())
				{
					FString GraphName = TEXT("MaterialGraph");
					if (MaterialGraph->RootNode.IsNull())
					{
						GraphName = MaterialGraph->GetName();
					}
				
					return FString::Printf(TEXT("%s-%s"), *Material->GetName(), *GraphName);
				}
			}

			return TEXT("InvalidMaterialEditor");
		}
		// End of TInnerPrinter interface.
	};
}
