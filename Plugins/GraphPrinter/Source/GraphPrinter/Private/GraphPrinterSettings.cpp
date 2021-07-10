// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinterSettings.h"
#include "ISettingsModule.h"
#include "ImageWriteTypes.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinterSettingsInternal
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

UGraphPrinterSettings::UGraphPrinterSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#if ENABLE_EMBED_NODE_INFO
	, bIsIncludeNodeInfoInImageFile(true)
#else
	, bIsIncludeNodeInfoInImageFile(false)
#endif
	, Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bUseGamma(true)
	, Padding(0.f)
	, MaxImageSize(15000.f, 15000.f)
	, bCanOverwriteFileWhenExport(false)
{
	OutputDirectory.Path = FPaths::Combine(FPaths::ProjectSavedDir(), GraphPrinterSettingsInternal::OutputDirectoryName);
}

void UGraphPrinterSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphPrinterSettingsInternal::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphPrinterSettingsInternal::ContainerName,
			GraphPrinterSettingsInternal::CategoryName,
			GraphPrinterSettingsInternal::SectionName,
			LOCTEXT("SettingName", "Graph Printer"),
			LOCTEXT("SettingDescription", "Editor settings for Graph Printer"),
			GetMutableDefault<UGraphPrinterSettings>()
		);
	}
}

void UGraphPrinterSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphPrinterSettingsInternal::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphPrinterSettingsInternal::ContainerName,
			GraphPrinterSettingsInternal::CategoryName,
			GraphPrinterSettingsInternal::SectionName
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

	FGraphPrinterCore::ShowNotification(
		LOCTEXT("EmbedNodeInfoDisabled", "This feature is under development.\nSee GraphPrinter.Build.cs to enable it."),
		FGraphPrinterCore::CS_Fail
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
