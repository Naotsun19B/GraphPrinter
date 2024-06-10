// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "GraphPrinterStyle.generated.h"

struct FSlateIcon;

// An enum class that defines type of icon registered in this style set.
UENUM()
enum class EGraphPrinterStyleIconType : uint8
{
	PluginIcon,
	CopyAllAreaOfWidgetToClipboard,
	CopySelectedAreaOfWidgetToClipboard,
	PrintAllAreaOfWidget,
	PrintSelectedAreaOfWidget,
	RestoreWidgetFromImageFile,
	OpenExportDestinationFolder,
};

namespace GraphPrinter
{
	/**
	 * A class that manages the slate icon used by this plugin.
	 */
	class GRAPHPRINTEREDITOREXTENSION_API FGraphPrinterStyle : public FSlateStyleSet
	{
	public:
		// Constructor.
		FGraphPrinterStyle();

	private:
		// The actual registration process for this class.
		void RegisterInternal();

	public:
		// Registers-Unregisters and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

		// Returns slate brush of specified icon type.
		static const FSlateBrush* GetBrushFromIconType(const EGraphPrinterStyleIconType IconType);

		// Returns slate icon of specified icon type.
		static FSlateIcon GetSlateIconFromIconType(const EGraphPrinterStyleIconType IconType);
		
		// Returns property name of specified icon type.
		static FName GetPropertyNameFromIconType(const EGraphPrinterStyleIconType IconType);
		
	private:
		// The instance of this style class.
		static TUniquePtr<FGraphPrinterStyle> Instance;
	};
}