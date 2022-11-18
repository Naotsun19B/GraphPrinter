// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

namespace GraphPrinter
{
	// The name of this plugin.
	static const FName PluginName = TEXT("GraphPrinter");
}

/**
 * Macro to support each engine version.
 */
#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION <= 0) || ENGINE_MAJOR_VERSION < 5
#define BEFORE_UE_5_00 1
#else
#define BEFORE_UE_5_00 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 27
#define BEFORE_UE_4_27 1
#else
#define BEFORE_UE_4_27 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
#define BEFORE_UE_4_24 1
#else
#define BEFORE_UE_4_24 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 23
#define BEFORE_UE_4_23 1
#else
#define BEFORE_UE_4_23 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 21
#define BEFORE_UE_4_21 1
#else
#define BEFORE_UE_4_21 0
#endif

/**
 * Categories used for log output with this plugin.
 */
GRAPHPRINTERGLOBALS_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphPrinter, Log, All);
