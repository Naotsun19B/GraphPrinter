// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GraphPrinterSettings"

namespace GraphPrinter
{
	namespace Settings
	{
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
{
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

void UGraphPrinterSettings::OpenSettings()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName
		);
	}
}

void UGraphPrinterSettings::FSubSettingsRegister::Register(UClass* Class)
{
	check(IsValid(Class) && Class->IsChildOf<UGraphPrinterSubSettings>());
	
	if (auto* This = GetMutableDefault<UGraphPrinterSettings>())
	{
		TArray<UClass*>& Classes = This->SubSettingsArray.SubSettingsClasses;
		if (!Classes.Contains(Class))
		{
			Classes.Add(Class);
		}
	}
}

#undef LOCTEXT_NAMESPACE
