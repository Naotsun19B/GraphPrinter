// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeprecatedReminderEditor/Types/DeprecatedRemindVerbosity.h"
#include "DeprecatedReminderSettings.generated.h"

/**
 * Editor settings for this plugin.
 */
UCLASS(Config = Editor, DefaultConfig)
class DEPRECATEDREMINDEREDITOR_API UDeprecatedReminderSettings : public UObject
{
	GENERATED_BODY()

public:
	// What to do with expired elements.
	UPROPERTY(EditAnywhere, Config, Category = "Verbosity")
	EDeprecatedRemindVerbosity DeprecatedRemindVerbosity;

	// What to do at compile time for expired code.
	UPROPERTY(EditAnywhere, Config, Category = "Verbosity", meta = (EditCondition = "bCodeBasedProject", HideEditConditionToggle))
	EDeprecatedRemindVerbosityAtCompileTime DeprecatedRemindVerbosityAtCompileTime;

protected:
	// An internal variable used to determine if bIsIncludeNodeInfoInImageFile can be edited.
	UPROPERTY()
	bool bCodeBasedProject;
	
public:
	// Constructor.
	UDeprecatedReminderSettings();
	
	// Register - unregister in the editor setting item.
	static void Register();
	static void Unregister();
	
	// Returns reference of this settings.
	static const UDeprecatedReminderSettings& Get();
};
