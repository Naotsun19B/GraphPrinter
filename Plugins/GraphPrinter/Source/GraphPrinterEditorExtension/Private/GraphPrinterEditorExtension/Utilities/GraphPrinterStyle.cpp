// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/Utilities/GraphPrinterStyle.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#if UE_5_00_OR_LATER
#include "Styling/SlateStyleMacros.h"
#include "Styling/CoreStyle.h"
#endif
#include "Textures/SlateIcon.h"
#include "Misc/Paths.h"

namespace GraphPrinter
{
#if !UE_5_00_OR_LATER
	namespace CoreStyleConstants
	{
		static const FVector2D Icon64x64(64.0f, 64.0f);
	}
#endif
	
	FGraphPrinterStyle::FGraphPrinterStyle()
		: FSlateStyleSet(TEXT("GraphPrinterStyle"))
	{
	}

#if !UE_5_00_OR_LATER
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#endif
	
	void FGraphPrinterStyle::RegisterInternal()
	{
		SetCoreContentRoot(FPaths::EngineContentDir());
		{
			FString StyleContentRoot;
			{
				const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(Global::PluginName.ToString());
				check(Plugin.IsValid());
				StyleContentRoot = FPaths::ConvertRelativePathToFull(
					Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icons")
				);
			}
			SetContentRoot(StyleContentRoot);
		}

		auto GetBrushName = [](const FString& Name) -> FString
		{
#if UE_5_00_OR_LATER
			return FString::Printf(TEXT("Gray%s"), *Name);
#else
			return Name;
#endif	
		};
		
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PluginIcon),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("Plugin128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::CopyAllAreaOfWidgetToClipboard),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("ClipboardAll128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::CopySelectedAreaOfWidgetToClipboard),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("ClipboardSelected128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PrintAllAreaOfWidget),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("PrintAll128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::PrintSelectedAreaOfWidget),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("PrintSelected128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::RestoreWidgetFromImageFile),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("Restore128")),
				CoreStyleConstants::Icon64x64
			)
		);
		Set(
			GetPropertyNameFromIconType(EGraphPrinterStyleIconType::OpenExportDestinationFolder),
			new IMAGE_BRUSH(
				GetBrushName(TEXT("Open128")),
				CoreStyleConstants::Icon64x64
			)
		);
	}

#if !UE_5_00_OR_LATER
#undef IMAGE_BRUSH
#endif
	
	void FGraphPrinterStyle::Register()
	{
		Instance = MakeUnique<FGraphPrinterStyle>();
		Instance->RegisterInternal();
		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

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

	const FSlateBrush* FGraphPrinterStyle::GetBrushFromIconType(const EGraphPrinterStyleIconType IconType)
	{
		return Get().GetBrush(GetPropertyNameFromIconType(IconType));
	}

	FSlateIcon FGraphPrinterStyle::GetSlateIconFromIconType(const EGraphPrinterStyleIconType IconType)
	{
		return FSlateIcon(Get().GetStyleSetName(), GetPropertyNameFromIconType(IconType));
	}

	FName FGraphPrinterStyle::GetPropertyNameFromIconType(const EGraphPrinterStyleIconType IconType)
	{
		const UEnum* EnumPtr = StaticEnum<EGraphPrinterStyleIconType>();
		check(IsValid(EnumPtr));
		
		const FString EnumName = EnumPtr->GetNameStringByValue(static_cast<int64>(IconType));
		return *FString::Printf(TEXT("%s.%s"), *Global::PluginName.ToString(), *EnumName);
	}

	TUniquePtr<FGraphPrinterStyle> FGraphPrinterStyle::Instance = nullptr;
}
