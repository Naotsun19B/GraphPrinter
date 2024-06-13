// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "MaterialGraphPrinter/WidgetPrinters/InnerMaterialGraphPrinter.h"
#include "PreviewViewportPrinter/WidgetPrinters/PreviewViewportPrinter.h"
#include "MaterialGraph/MaterialGraph.h"
#include "SGraphEditorImpl.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SImage.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"
#if !UE_5_00_OR_LATER
#include "MaterialGraph/MaterialGraphNode_Root.h"
#endif

namespace GraphPrinter
{
	FMaterialGraphPrinter::FMaterialGraphPrinter(UPrintWidgetOptions* InPrintOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InPrintOptions, InOnPrinterProcessingFinished)
	{
	}
	
	FMaterialGraphPrinter::FMaterialGraphPrinter(URestoreWidgetOptions* InRestoreOptions, const FSimpleDelegate& InOnPrinterProcessingFinished)
		: Super(InRestoreOptions, InOnPrinterProcessingFinished)
	{
	}

	bool FMaterialGraphPrinter::CanPrintWidget() const
	{
		if (Super::CanPrintWidget())
		{
			const TSharedPtr<SGraphEditorImpl> GraphEditor = FindTargetWidget(PrintOptions->SearchTarget);
			return IsValid(Cast<UMaterialGraph>(GraphEditor->GetCurrentGraph()));
		}

		return false;
	}

	UTextureRenderTarget2D* FMaterialGraphPrinter::DrawWidgetToRenderTarget()
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

		// Re-renders the widget that concatenates the two render targets.
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

#if UE_5_00_OR_LATER
		CombinedWidget->MarkPrepassAsDirty();
#else
		CombinedWidget->InvalidatePrepass();
#endif

		const FVector2D CombinedSize(
			RenderingResult.RenderTarget->SizeX + RenderedGraph->SizeX,
			FMath::Max(RenderingResult.RenderTarget->SizeY, RenderedGraph->SizeY)
		);
		
		return DrawWidgetToRenderTargetInternal(
			CombinedWidget,
			CombinedSize,
			PrintOptions->FilteringMode,
			false, // If draws with gamma twice, it will be too bright, so gamma is not used here.
			PrintOptions->RenderingScale
		);
	}

	FString FMaterialGraphPrinter::GetWidgetTitle()
	{
		FString Title;
		GetMaterialGraphTitle(Widget, Title);
		return Title;
	}

	bool FMaterialGraphPrinter::GetMaterialGraphTitle(const TSharedPtr<SGraphEditorImpl>& MaterialGraphEditor, FString& Title)
	{
		Title = TEXT("InvalidMaterialEditor");

		const auto* MaterialGraph = Cast<UMaterialGraph>(MaterialGraphEditor->GetCurrentGraph());
		if (!IsValid(MaterialGraph))
		{
			return false;
		}

		const UMaterial* Material = 
#if UE_5_00_OR_LATER
			MaterialGraph->Material.Get();
#else
			MaterialGraph->Material;
#endif
		if (!IsValid(Material))
		{
			return false;
		}
		
		FString GraphName = TEXT("MaterialGraph");
#if UE_5_00_OR_LATER
		if (MaterialGraph->RootNode)
#else
		if (IsValid(MaterialGraph->RootNode))
#endif
		{
			GraphName = MaterialGraph->GetName();
		}
				
		Title = FString::Printf(TEXT("%s-%s"), *Material->GetName(), *GraphName);
		return true;
	}
}
