// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphPrinterSettings.h"
#include "GraphPrinterGlobals.h"
#include "ISettingsModule.h"
#include "ImageWriteTypes.h"

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinterSettingsInternal
{
	static const FString OutputDirectoryName = TEXT("GraphPrinter");

	ISettingsModule* GetSettingsModule()
	{
		return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	}
}

UGraphPrinterSettings::UGraphPrinterSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bUseGamma(true)
	, bCanOverwriteFileWhenExport(false)
	, OutputDirectoryPath(FPaths::Combine(FPaths::ProjectSavedDir(), GraphPrinterSettingsInternal::OutputDirectoryName))
{
}

void UGraphPrinterSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphPrinterSettingsInternal::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			"Editor",
			"GraphPrinter",
			"GraphPrinterSettings",
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
			"Editor", 
			"GraphPrinter",
			"GraphPrinterSettings"
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
