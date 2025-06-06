// Copyright 2020-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GraphPrinterSettings.generated.h"

/**
 * An editor preferences class for this plugin.
 */
UCLASS(Abstract, Config = Editor, GlobalUserConfig)
class GRAPHPRINTERGLOBALS_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// Registers in the editor setting item.
	static void Register();
	
	// Returns the name of the section registered in ISettingsModule.
	virtual FName GetSectionName() const;

	// Returns the name of the settings that will be displayed in the editor.
	virtual FText GetDisplayName() const;

	// Returns tooltip text for settings displayed in the editor.
	virtual FText GetTooltipText() const;

	// Opens the settings menu for this plugin.
	static void OpenSettings(const FName SectionName);

	// Returns all registered editor settings classes about GraphPrinter.
	static const TArray<UGraphPrinterSettings*>& GetAllSettings();

protected:
	// Returns the unique name of this setting.
	virtual FString GetSettingsName() const PURE_VIRTUAL(UGraphPrinterSettings::GetSettingsName, return {};)

	// Called before saving to the config file.
	virtual void PreSaveConfig() {}
	
private:
	// Called when the end of UEngine::Init, right before loading PostEngineInit modules for both normal execution and commandlets.
	static void HandleOnPostEngineInit();

	// Called before the engine exits. Separate from OnPreExit as OnEnginePreExit occurs before shutting down any core modules.
	static void HandleOnEnginePreExit();

private:
	// The list of all registered editor settings classes about this plugin.
	static TArray<UGraphPrinterSettings*> AllSettings;
};

namespace GraphPrinter
{
	// Returns reference of this settings.
	template<class TSettings>
	static const TSettings& GetSettings()
	{
		const auto* Settings = GetDefault<TSettings>();
		check(IsValid(Settings));
		return *Settings;
	}
	
	// Opens the settings menu for this plugin.
	template<class TSettings>
	static void OpenSettings()
	{
		static_assert(TIsDerivedFrom<TSettings, UGraphPrinterSettings>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UGraphPrinterSettings.");

		const auto& Settings = GetSettings<TSettings>();
		UGraphPrinterSettings::OpenSettings(Settings.GetSectionName());
	}
}
