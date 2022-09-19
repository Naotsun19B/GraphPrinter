// Copyright 2022 Naotsun. All Rights Reserved.

#include "DeprecatedReminderEditor/Utilities/DeprecatedReminderSettings.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "Interfaces/IProjectManager.h"

#define LOCTEXT_NAMESPACE "DeprecatedReminderSettings"

namespace DeprecatedReminder
{
	namespace Settings
	{
		static const FName ContainerName    = TEXT("Editor");
		static const FName CategoryName	    = TEXT("Plugins");
		static const FName SectionName      = TEXT("DeprecatedReminderSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

UDeprecatedReminderSettings::UDeprecatedReminderSettings()
	: DeprecatedRemindVerbosity(EDeprecatedRemindVerbosity::WarningAtCook)
	, DeprecatedRemindVerbosityAtCompileTime(EDeprecatedRemindVerbosityAtCompileTime::Warning)
	, bCodeBasedProject(true)
{
	FProjectStatus ProjectStatus;
	if (IProjectManager::Get().QueryStatusForCurrentProject(ProjectStatus))
	{
		bCodeBasedProject = ProjectStatus.bCodeBasedProject;
	}
}

void UDeprecatedReminderSettings::Register()
{
	if (ISettingsModule* SettingsModule = DeprecatedReminder::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			DeprecatedReminder::Settings::ContainerName,
			DeprecatedReminder::Settings::CategoryName,
			DeprecatedReminder::Settings::SectionName,
			LOCTEXT("SettingName", "Deprecated Reminder"),
			LOCTEXT("SettingDescription", "Editor settings for Deprecated Reminder"),
			GetMutableDefault<UDeprecatedReminderSettings>()
		);
	}
}

void UDeprecatedReminderSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = DeprecatedReminder::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			DeprecatedReminder::Settings::ContainerName,
			DeprecatedReminder::Settings::CategoryName,
			DeprecatedReminder::Settings::SectionName
		);
	}
}

const UDeprecatedReminderSettings& UDeprecatedReminderSettings::Get()
{
	const auto* Settings = GetDefault<UDeprecatedReminderSettings>();
	check(IsValid(Settings));
	return *Settings;
}

#undef LOCTEXT_NAMESPACE
