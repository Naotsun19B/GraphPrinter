// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "UObject/UObjectIterator.h"

namespace GraphPrinter
{
	namespace Settings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		}
	}
}

UGraphPrinterSettings::FSettingsInfo::FSettingsInfo(const FName& InSectionName)
	: SectionName(InSectionName)
{
	DisplayName = FText::FromString(FName::NameToDisplayString(SectionName.ToString(), false));
}

UGraphPrinterSettings::FSettingsInfo::FSettingsInfo(
	const FName& InSectionName,
	const FText& InDisplayName,
	const FText& InDescription
)
	: SectionName(InSectionName)
	, DisplayName(InDisplayName)
	, Description(InDescription)
{
}

FText UGraphPrinterSettings::FSettingsInfo::GetFormattedDisplayName() const
{
	return FText::Format(
		NSLOCTEXT("GraphPrinterSettings", "DisplayNameFormat", "{0} - {1}"),
		FText::FromString(FName::NameToDisplayString(GraphPrinter::PluginName.ToString(), false)),
		DisplayName
	);
}

void UGraphPrinterSettings::Register()
{
	IMainFrameModule::Get().OnMainFrameCreationFinished().AddStatic(&UGraphPrinterSettings::HandleOnMainFrameCreationFinished);
}

void UGraphPrinterSettings::Unregister()
{
	ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}

	for (const auto& Settings : AllSettings)
	{
		SettingsModule->UnregisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			Settings.SectionName
		);
	}
}

const TArray<UGraphPrinterSettings::FSettingsInfo>& UGraphPrinterSettings::GetAllSettings()
{
	return AllSettings;
}

void UGraphPrinterSettings::OpenSettings(FName SectionName)
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			SectionName
		);
	}
}

void UGraphPrinterSettings::HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
	ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}
	
	for (auto* GraphPrinterSettings : TObjectRange<UGraphPrinterSettings>(RF_NoFlags))
	{
		if (!IsValid(GraphPrinterSettings))
		{
			continue;
		}

		const UClass* SettingsClass = GraphPrinterSettings->GetClass();
		if (!IsValid(SettingsClass))
		{
			continue;
		}
		if (SettingsClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}

		FSettingsInfo SettingsInfo = GraphPrinterSettings->GetSettingsInfo();
		if (SettingsInfo.Description.IsEmpty())
		{
			SettingsInfo.Description = SettingsClass->GetToolTipText();
		}
		
		SettingsModule->RegisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			SettingsInfo.SectionName,
			SettingsInfo.GetFormattedDisplayName(),
			SettingsInfo.Description,
			GraphPrinterSettings
		);

		AllSettings.Add(SettingsInfo);
	}
}

TArray<UGraphPrinterSettings::FSettingsInfo> UGraphPrinterSettings::AllSettings;
