// Copyright 2020 Naotsun. All Rights Reserved.

#include "GraphPrinterSettings.h"
#include "GraphPrinterGlobals.h"
#include "ISettingsModule.h"
#include "ImageWriteTypes.h"

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
	, bIsIncludeNodeInfoInImageFile(true)
	, Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bUseGamma(true)
	, Padding(0.f)
	, bCanOverwriteFileWhenExport(false)
	, OutputDirectoryPath(FPaths::Combine(FPaths::ProjectSavedDir(), GraphPrinterSettingsInternal::OutputDirectoryName))
{
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

void UGraphPrinterSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, bIsIncludeNodeInfoInImageFile))
	{
		if (bIsIncludeNodeInfoInImageFile)
		{
			Format = EDesiredImageFormat::PNG;
		}
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, Format))
	{
		if (Format == EDesiredImageFormat::EXR)
		{
			CompressionQuality = 0;
		}
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, CompressionQuality))
	{
		int32 Max = 100;
		if (Format == EDesiredImageFormat::EXR)
		{
			Max = 1;
		}
		CompressionQuality = FMath::Clamp(CompressionQuality, 0, Max);
	}
}

#undef LOCTEXT_NAMESPACE
