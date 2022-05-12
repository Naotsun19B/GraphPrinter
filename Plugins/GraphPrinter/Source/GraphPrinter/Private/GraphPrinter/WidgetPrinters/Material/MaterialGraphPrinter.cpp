// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinter/WidgetPrinters/Material/MaterialGraphPrinter.h"
#include "GraphPrinter/WidgetPrinters/PreviewViewport/PreviewViewportPrinter.h"
#include "GraphPrinter/Utilities/WidgetPrinterUtils.h"
#include "GraphPrinter/Utilities/CastSlateWidget.h"
#include "SGraphEditorImpl.h"
#include "MaterialGraph/MaterialGraph.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Toolkits/SStandaloneAssetEditorToolkitHost.h"

bool UMaterialGraphPrinter::CanPrintWidget(const GraphPrinter::FPrintWidgetOptions& Options) const
{
	const TSharedPtr<SGraphEditorImpl> GraphEditor = FindGraphEditor(Options.TargetWidget);
	if (GraphEditor.IsValid())
	{
		return IsValid(Cast<UMaterialGraph>(GraphEditor->GetCurrentGraph()));
	}

	return false;
}

int32 UMaterialGraphPrinter::GetPriority() const
{
	return MaterialGraphPrinterPrinter;
}

FString UMaterialGraphPrinter::GetWidgetTitle(const TSharedPtr<SWidget>& Widget) const
{
	const TSharedPtr<SGraphEditorImpl>& GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, Widget);
	if (GraphEditor.IsValid())
	{
		if (const auto* MaterialGraph = Cast<UMaterialGraph>(GraphEditor->GetCurrentGraph()))
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
	}

	return TEXT("InvalidMaterialEditor");
}

UTextureRenderTarget2D* UMaterialGraphPrinter::DrawWidgetToRenderTarget(
	const TSharedPtr<SWidget>& Widget,
	const FVector2D& DrawSize,
	const GraphPrinter::FPrintWidgetOptions& Options
)
{
	UTextureRenderTarget2D* RenderedGraph = Super::DrawWidgetToRenderTarget(Widget, DrawSize, Options);
	if (!IsValid(RenderedGraph))
	{
		return nullptr;
	}

	if (Options.MaterialGraphExportMethod == EMaterialGraphExportMethod::GraphOnly)
	{
		return RenderedGraph;
	}

	const TSharedPtr<SStandaloneAssetEditorToolkitHost> StandaloneAssetEditorToolkitHost = GraphPrinter::FWidgetPrinterUtils::FindNearestParentStandaloneAssetEditorToolkitHost(Widget);

	if (Options.MaterialGraphExportMethod == EMaterialGraphExportMethod::PreviewAndGraphSeparately)
	{
		if (auto* PreviewViewportPrinter = GetMutableDefault<UPreviewViewportPrinter>())
		{
			GraphPrinter::FPrintWidgetOptions OptionWithTargetWidget = Options;
			OptionWithTargetWidget.TargetWidget = StandaloneAssetEditorToolkitHost;
			PreviewViewportPrinter->PrintWidget(OptionWithTargetWidget);
			return RenderedGraph;
		}
	}

	UTextureRenderTarget2D* RenderedPreviewViewport = UPreviewViewportPrinter::GetRenderedPreviewViewport(StandaloneAssetEditorToolkitHost, Options);
	if (!IsValid(RenderedPreviewViewport))
	{
		return nullptr;
	}

	// Re-render the widget that concatenates the two render targets.
	const TSharedPtr<FSlateBrush> PreviewViewportBrush = MakeShared<FSlateBrush>();
	PreviewViewportBrush->SetResourceObject(RenderedPreviewViewport);
	const TSharedPtr<FSlateBrush> GraphBrush = MakeShared<FSlateBrush>();
	GraphBrush->SetResourceObject(RenderedGraph);

	const TSharedPtr<SWidget> CombinedWidget = SNew(SHorizontalBox)
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

	GraphPrinter::FPrintWidgetOptions ToggledGamma = Options;
	ToggledGamma.bUseGamma = !ToggledGamma.bUseGamma;
	
	return Super::DrawWidgetToRenderTarget(
		CombinedWidget,
		CombinedSize,
		ToggledGamma
	);
}
