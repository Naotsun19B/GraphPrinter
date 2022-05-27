// Copyright 2020-2022 Naotsun. All Rights Reserved.

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

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Instance->RootToContentDir(TEXT(RelativePath), TEXT(".png")), __VA_ARGS__)

	void FGraphPrinterStyle::Register()
	{
		FString StyleContentRoot;
		{
			const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(GraphPrinter::PluginName.ToString());
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
#if BEFORE_UE_4_27
			new IMAGE_BRUSH("Icon128", IconSize::Icon64x64)
#else
			new IMAGE_BRUSH("GrayIcon128", IconSize::Icon64x64)
#endif
		);

		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

#undef IMAGE_BRUSH

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
		return *FString::Printf(TEXT("Icons.%s"), *EnumName);
	}

	TSharedPtr<FGraphPrinterStyle> FGraphPrinterStyle::Instance = nullptr;
}
