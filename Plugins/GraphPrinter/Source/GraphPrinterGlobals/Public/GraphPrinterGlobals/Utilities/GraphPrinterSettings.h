// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GraphPrinterSettings.generated.h"

/**
 * Editor settings for this plugin.
 */
UCLASS(Abstract, Config = Editor)
class GRAPHPRINTERGLOBALS_API UGraphPrinterSettings : public UObject
{
	GENERATED_BODY()

public:
	// Information on registered settings.
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
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();

	// Returns all registered editor settings classes about GraphPrinter.
	static const TArray<FSettingsInfo>& GetAllSettings();
	
	// Open the settings menu for this plugin.
	static void OpenSettings(FName SectionName);

	// The section name to register in the editor preference.
	virtual FSettingsInfo GetSettingsInfo() const PURE_VIRTUAL(UGraphPrinterSettings::GetSectionName, { return FSettingsInfo(NAME_None); });
	
private:
	// Called when the editor mainframe has been created.
	static void HandleOnMainFrameCreationFinished(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);

private:
	// All registered editor settings classes about GraphPrinter.
	static TArray<FSettingsInfo> AllSettings;
};