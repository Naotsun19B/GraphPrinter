// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"
#include "Misc/CoreDelegates.h"
#include "UObject/UObjectIterator.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphPrinterSettings)
#endif

#define LOCTEXT_NAMESPACE "GraphPrinterSettings"

namespace GraphPrinter
{
	namespace Settings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

void UGraphPrinterSettings::Register()
{
	FCoreDelegates::OnPostEngineInit.AddStatic(&UGraphPrinterSettings::HandleOnPostEngineInit);
	FCoreDelegates::OnEnginePreExit.AddStatic(&UGraphPrinterSettings::HandleOnEnginePreExit);
}

FName UGraphPrinterSettings::GetSectionName() const
{
	return *(GraphPrinter::Global::PluginName.ToString() + GetSettingsName());
}

FText UGraphPrinterSettings::GetDisplayName() const
{
	return FText::Format(
		LOCTEXT("DisplayNameFormat", "{0} - {1}"),
		FText::FromString(FName::NameToDisplayString(GraphPrinter::Global::PluginName.ToString(), false)),
		FText::FromString(GetSettingsName())
	);
}

FText UGraphPrinterSettings::GetTooltipText() const
{
	const UClass* SettingsClass = GetClass();
	check(IsValid(SettingsClass));
	return SettingsClass->GetToolTipText();
}

void UGraphPrinterSettings::OpenSettings(const FName SectionName)
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

const TArray<UGraphPrinterSettings*>& UGraphPrinterSettings::GetAllSettings()
{
	return AllSettings;
}

void UGraphPrinterSettings::HandleOnPostEngineInit()
{
	ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}
	
	for (auto* Settings : TObjectRange<UGraphPrinterSettings>(RF_NoFlags))
	{
		if (!IsValid(Settings))
		{
			continue;
		}

		const UClass* SettingsClass = Settings->GetClass();
		if (!IsValid(SettingsClass))
		{
			continue;
		}
		if (SettingsClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		if (!Settings->IsTemplate())
		{
			continue;
		}
		
		SettingsModule->RegisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			Settings->GetSectionName(),
			Settings->GetDisplayName(),
			Settings->GetTooltipText(),
			Settings
		);

		Settings->AddToRoot();
		AllSettings.Add(Settings);
	}
}

void UGraphPrinterSettings::HandleOnEnginePreExit()
{
	ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}

	for (auto* Settings : AllSettings)
	{
		Settings->PreSaveConfig();
		
		const TSharedPtr<ISettingsContainer> Container = SettingsModule->GetContainer(GraphPrinter::Settings::ContainerName);
		check(Container.IsValid());
		const TSharedPtr<ISettingsCategory> Category = Container->GetCategory(GraphPrinter::Settings::CategoryName);
		check(Category.IsValid());
		const TSharedPtr<ISettingsSection> Section = Category->GetSection(Settings->GetSectionName());
		check(Section.IsValid());
		Section->Save();
		
		SettingsModule->UnregisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			Settings->GetSectionName()
		);

		Settings->RemoveFromRoot();
	}
}

TArray<UGraphPrinterSettings*> UGraphPrinterSettings::AllSettings;
