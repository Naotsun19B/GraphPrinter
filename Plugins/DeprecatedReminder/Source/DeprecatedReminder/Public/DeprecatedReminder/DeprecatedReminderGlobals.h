// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

namespace DeprecatedReminder
{
	// The name of this plugin.
	static const FName PluginName = TEXT("DeprecatedReminder");
}

/**
 * Macro to support each engine version.
 */
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 27
#define BEFORE_UE_4_27 1
#else
#define BEFORE_UE_4_27 0
#endif

/**
 * Categories used for log output with this plugin.
 */
DEPRECATEDREMINDER_API DECLARE_LOG_CATEGORY_EXTERN(LogDeprecatedReminder, Log, All);
