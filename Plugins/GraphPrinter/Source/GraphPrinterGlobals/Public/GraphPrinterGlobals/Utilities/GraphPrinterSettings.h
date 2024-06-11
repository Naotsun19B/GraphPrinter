// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSettings.generated.h"

/**
 * A editor preferences class for this plugin.
 */
UCLASS(Abstract, Config = Editor, GlobalUserConfig)
class GRAPHPRINTERGLOBALS_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()

public:
	// A struct that information on registered settings.
	struct GRAPHPRINTERGLOBALS_API FSettingsInfo
	{
	public:
		// Constructor.
		explicit FSettingsInfo(const FName& InSectionName);
		FSettingsInfo(
			const FName& InSectionName,
			const FText& InDisplayName,
			const FText& InDescription
		);

		// Returns text with the name of the plugin at the beginning of the display name.
		FText GetFormattedDisplayName() const;
	
	public:
		// The section name to register in the editor preference.
		FName SectionName;

		// The display name in the editor preference of section name.
		FText DisplayName;

		// The tooltip text displayed in the editor preferences.
		FText Description;
	};
	
public:
	// Registers-Unregisters in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns all registered editor settings classes about GraphPrinter.
	static const TArray<FSettingsInfo>& GetAllSettings();
	
	// Opens the settings menu for this plugin.
	static void OpenSettings(const FName SectionName);

	// Returns the section name to register in the editor preference.
	virtual FSettingsInfo GetSettingsInfo() const PURE_VIRTUAL(UGraphPrinterSettings::GetSectionName, return FSettingsInfo(NAME_None);)
	
private:
	// Called when the end of UEngine::Init, right before loading PostEngineInit modules for both normal execution and commandlets
	static void HandleOnPostEngineInit();

private:
	// The list of all registered editor settings classes about GraphPrinter.
	static TArray<FSettingsInfo> AllSettings;
};

namespace GraphPrinter
{
	// Opens the settings menu for this plugin.
	template<class TSettings>
	static void OpenSettings()
	{
		static_assert(TIsDerivedFrom<TSettings, UGraphPrinterSettings>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of UGraphPrinterSettings.");

		auto* Settings = GetDefault<TSettings>();
		UGraphPrinterSettings::OpenSettings(Settings->GetSettingsInfo().SectionName);
	}
}
