// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "GraphPrinterStyle.generated.h"

struct FSlateIcon;

// The type of icon registered in this style set.
UENUM()
enum class EGraphPrinterStyleIconType : uint8
{
	PluginIcon,
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
		
		// Register-Unregister and instance getter this class.
		static void Register();
		static void Unregister();
		static const ISlateStyle& Get();

		// Returns slate brush of specified icon type.
		static const FSlateBrush* GetBrushFromIconType(EGraphPrinterStyleIconType IconType);

		// Returns slate icon of specified icon type.
		static FSlateIcon GetSlateIconFromIconType(EGraphPrinterStyleIconType IconType);
		
		// Returns property name of specified icon type.
		static FName GetPropertyNameFromIconType(EGraphPrinterStyleIconType IconType);
		
	private:
		// An instance of this style class.
		static TSharedPtr<FGraphPrinterStyle> Instance;
	};
}