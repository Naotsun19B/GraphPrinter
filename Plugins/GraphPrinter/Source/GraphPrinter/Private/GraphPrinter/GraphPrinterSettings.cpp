// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinter/GraphPrinterSettings.h"
#include "GraphPrinter/GraphPrinterGlobals.h"
#include "ISettingsModule.h"
#include "ImageWriteTypes.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture.h"
#include "Misc/Paths.h"

#if !ENABLE_EMBED_NODE_INFO
#include "GraphPrinter/GraphPrinterCore.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinter
{
	namespace Settings
	{
		static const FString OutputDirectoryName	= TEXT("GraphPrinter");

		static const FName ContainerName			= TEXT("Editor");
		static const FName CategoryName				= TEXT("Plugins");
		static const FName SectionName				= TEXT("GraphPrinterSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		}
	}
}

UGraphPrinterSettings::UGraphPrinterSettings()
#if ENABLE_EMBED_NODE_INFO
	: bIsIncludeNodeInfoInImageFile(true)
#else
	: bIsIncludeNodeInfoInImageFile(false)
#endif
	, Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bDrawOnlyGraph(false)
	, bUseGamma(true)
	, Padding(0.f)
	, MaxImageSize(15000.f, 15000.f)
	, bCanOverwriteFileWhenExport(false)
{
	OutputDirectory.Path = FPaths::Combine(FPaths::ProjectSavedDir(), GraphPrinter::Settings::OutputDirectoryName);
}

const UGraphPrinterSettings& UGraphPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UGraphPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UGraphPrinterSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName,
			LOCTEXT("SettingName", "Graph Printer"),
			LOCTEXT("SettingDescription", "Editor settings for Graph Printer"),
			GetMutableDefault<UGraphPrinterSettings>()
		);
	}
}

void UGraphPrinterSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName
		);
	}
}

void UGraphPrinterSettings::PostInitProperties()
{
	Super::PostInitProperties();

#if !ENABLE_EMBED_NODE_INFO
	bIsIncludeNodeInfoInImageFile = false;
#endif

	ModifyFormat();
	ModifyCompressionQuality();
}

void UGraphPrinterSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, bIsIncludeNodeInfoInImageFile))
	{
		ModifyFormat();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, Format) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, CompressionQuality))
	{
		ModifyCompressionQuality();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, MaxImageSize))
	{
		if (MaxImageSize.X < 0.f)
		{
			MaxImageSize.X = 0.f;
		}
		if (MaxImageSize.Y < 0.f)
		{
			MaxImageSize.Y = 0.f;
		}
	}
}

void UGraphPrinterSettings::ModifyFormat()
{
#if ENABLE_EMBED_NODE_INFO
	if (bIsIncludeNodeInfoInImageFile)
	{
		Format = EDesiredImageFormat::PNG;
	}
#else
	bIsIncludeNodeInfoInImageFile = false;

	GraphPrinter::FGraphPrinterCore::ShowNotification(
		LOCTEXT("EmbedNodeInfoDisabled", "This feature is under development.\nSee GraphPrinter.Build.cs to enable it."),
		GraphPrinter::FGraphPrinterCore::CS_Fail
	);
#endif
}

void UGraphPrinterSettings::ModifyCompressionQuality()
{
	int32 Max = 100;
	if (Format == EDesiredImageFormat::EXR)
	{
		Max = 1;
	}
	CompressionQuality = FMath::Clamp(CompressionQuality, 0, Max);
}

#undef LOCTEXT_NAMESPACE
