// Copyright 2022 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "DeprecatedReminder/DeprecatedReminderGlobals.h"

#if WITH_EDITOR
#include "Interfaces/IProjectManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#endif

DEFINE_LOG_CATEGORY(LogDeprecatedReminder);

#if WITH_EDITOR
#define GET_NAMESPACE_NAME(NameSpace) ((void)sizeof(##NameSpace##::FDeprecatedReminderModule), TEXT(#NameSpace))
#endif

namespace DeprecatedReminder
{
	class FDeprecatedReminderModule : public IModuleInterface
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

#if WITH_EDITOR
	private:
		// Do nothing for a project without code.
		bool bCodeBasedProject = false;
#endif
	};
	
	void FDeprecatedReminderModule::StartupModule()
	{
#if WITH_EDITOR
		FProjectStatus ProjectStatus;
		if (IProjectManager::Get().QueryStatusForCurrentProject(ProjectStatus))
		{
			bCodeBasedProject = ProjectStatus.bCodeBasedProject;
		}

		if (!bCodeBasedProject)
		{
			return;
		}
		
		const FDateTime CompileTime(
			DEPRECATED_REMINDER_COMPILE_TIME_YEAR,
			DEPRECATED_REMINDER_COMPILE_TIME_MONTH,
			DEPRECATED_REMINDER_COMPILE_TIME_DAY,
			DEPRECATED_REMINDER_COMPILE_TIME_HOUR,
			DEPRECATED_REMINDER_COMPILE_TIME_MINUTE
		);
		UE_LOG(LogDeprecatedReminder, Log, TEXT("Date and time at compile time : %s"), *CompileTime.ToString());
#endif
	}

	void FDeprecatedReminderModule::ShutdownModule()
	{
#if WITH_EDITOR
		if (!bCodeBasedProject)
		{
			return;
		}
		
		// Since Build.cs is not executed every time you build a project with an editor,
		// as a countermeasure, change Build.cs at the end of the program so that
		// Build.cs will be executed the next time you build.
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName.ToString());
		if (!Plugin.IsValid())
		{
			return;
		}

		const FString ModuleName = GET_NAMESPACE_NAME(DeprecatedReminder);
		const FString BuildCsPath = Plugin->GetBaseDir() / TEXT("Source") / ModuleName / ModuleName + TEXT(".Build.cs");

		TArray<FString> BuildCsLines;
		if (!FFileHelper::LoadFileToStringArray(BuildCsLines, *BuildCsPath))
		{
			return;
		}

		const FString Timestamp = FString::Printf(TEXT("// %s"), *FDateTime::Now().ToString());
		FString& Last = BuildCsLines.Last();
		if (Last.Len() < 2 || !(Last[0] == '/' && Last[1] == '/'))
		{
			BuildCsLines.Add(Timestamp);
		}
		else
		{
			Last = Timestamp;
		}

		FFileHelper::SaveStringArrayToFile(BuildCsLines, *BuildCsPath, FFileHelper::EEncodingOptions::ForceUTF8);
#endif
	}	
}

#if WITH_EDITOR
#undef GET_NAMESPACE_NAME
#endif

IMPLEMENT_MODULE(DeprecatedReminder::FDeprecatedReminderModule, DeprecatedReminder)
