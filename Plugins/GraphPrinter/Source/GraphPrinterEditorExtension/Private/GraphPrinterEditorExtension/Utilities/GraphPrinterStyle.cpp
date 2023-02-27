// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Textures/SlateIcon.h"

namespace GraphPrinter
{
	namespace IconSize
	{
		static const FVector2D Icon64x64(64.0f, 64.0f);
	}
	
	FGraphPrinterStyle::FGraphPrinterStyle()
		: FSlateStyleSet(TEXT("GraphPrinterStyle"))
	{
	}

#define IMAGE_BRUSH_INTERNAL(RelativePath, ...) FSlateImageBrush(Instance->RootToContentDir(TEXT(RelativePath), TEXT(".png")), __VA_ARGS__)

#if UE_5_00_OR_LATER
#define IMAGE_BRUSH(RelativePath, ...) IMAGE_BRUSH_INTERNAL("Gray" RelativePath, __VA_ARGS__)
#else
#define IMAGE_BRUSH(RelativePath, ...) IMAGE_BRUSH_INTERNAL(RelativePath, __VA_ARGS__)
#endif
	
	void FGraphPrinterStyle::Register()
	{
		FString StyleContentRoot;
		{
			const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName.ToString());
			check(Plugin.IsValid());
			StyleContentRoot = FPaths::ConvertRelativePathToFull(
				Plugin->GetBaseDir() / TEXT("Resources")
			);
		}
		
		Instance = MakeShared<FGraphPrinterStyle>();
		Instance->SetContentRoot(StyleContentRoot);
		Instance->SetCoreContentRoot(StyleContentRoot);
		
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PluginIcon),
			new IMAGE_BRUSH("Icon128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::CopyAllAreaOfWidgetToClipboard),
			new IMAGE_BRUSH("ClipboardAll128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::CopySelectedAreaOfWidgetToClipboard),
			new IMAGE_BRUSH("ClipboardSelected128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PrintAllAreaOfWidget),
			new IMAGE_BRUSH("PrintAll128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PrintSelectedAreaOfWidget),
			new IMAGE_BRUSH("PrintSelected128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::RestoreWidgetFromImageFile),
			new IMAGE_BRUSH("Restore128", IconSize::Icon64x64)
		);
		Instance->Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::OpenExportDestinationFolder),
			new IMAGE_BRUSH("Open128", IconSize::Icon64x64)
		);

		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

#undef IMAGE_BRUSH
#undef IMAGE_BRUSH_INTERNAL

	void FGraphPrinterStyle::Unregister()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Instance);
		Instance.Reset();
	}

	const ISlateStyle& FGraphPrinterStyle::Get()
	{
		check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
		return *Instance.Get();
	}

	const FSlateBrush* FGraphPrinterStyle::GetBrushFromIconType(EGraphPrinterStyleIconType IconType)
	{
		return Get().GetBrush(GetPropertyNameFromIconType(IconType));
	}

	FSlateIcon FGraphPrinterStyle::GetSlateIconFromIconType(EGraphPrinterStyleIconType IconType)
	{
		return FSlateIcon(Get().GetStyleSetName(), GetPropertyNameFromIconType(IconType));
	}

	FName FGraphPrinterStyle::GetPropertyNameFromIconType(EGraphPrinterStyleIconType IconType)
	{
		const UEnum* EnumPtr = StaticEnum<EGraphPrinterStyleIconType>();
		check(IsValid(EnumPtr));
		
		const FString EnumName = EnumPtr->GetNameStringByValue(static_cast<int64>(IconType));
		return *FString::Printf(TEXT("%s.%s"), *PluginName.ToString(), *EnumName);
	}

	TSharedPtr<FGraphPrinterStyle> FGraphPrinterStyle::Instance = nullptr;
}
