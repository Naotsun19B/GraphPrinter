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
#ifdef ENABLE_EMBED_NODE_INFO
	, bIsIncludeNodeInfoInImageFile(true)
#else
	, bIsIncludeNodeInfoInImageFile(false)
#endif
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

void UGraphPrinterSettings::PostInitProperties()
{
	Super::PostInitProperties();

#ifndef ENABLE_EMBED_NODE_INFO
	bIsIncludeNodeInfoInImageFile = false;
#endif

	ModifyFormat();
	ModifyCompressionQuality();
}

bool UGraphPrinterSettings::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEditChange = true;

	if (InProperty != nullptr)
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, bIsIncludeNodeInfoInImageFile))
		{
#if ENABLE_EMBED_NODE_INFO
			bCanEditChange = true;
#else
			bCanEditChange = false;
#endif
		}
	}


	return Super::CanEditChange(InProperty) && bCanEditChange;
}

void UGraphPrinterSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

#if ENABLE_EMBED_NODE_INFO
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, bIsIncludeNodeInfoInImageFile))
	{
		ModifyFormat();
	}
#endif

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, Format) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, CompressionQuality))
	{
		ModifyCompressionQuality();
	}
}

void UGraphPrinterSettings::ModifyFormat()
{
#ifdef ENABLE_EMBED_NODE_INFO
	if (bIsIncludeNodeInfoInImageFile)
	{
		Format = EDesiredImageFormat::PNG;
	}
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
