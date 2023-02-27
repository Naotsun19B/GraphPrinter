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
#define ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion) (MajorVersion * 100 + MinorVersion)
#define COMPARE_ENGINE_VERSION(MajorVersion, MinorVersion) ENGINE_VERSION_NUMBER(ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION) >= ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion)

#if COMPARE_ENGINE_VERSION(5, 1)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif

#if COMPARE_ENGINE_VERSION(5, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif

#if COMPARE_ENGINE_VERSION(4, 25)
#define UE_4_25_OR_LATER 1
#else
#define UE_4_25_OR_LATER 0
#endif

#if COMPARE_ENGINE_VERSION(4, 24)
#define UE_4_24_OR_LATER 1
#else
#define UE_4_24_OR_LATER 0
#endif

#if COMPARE_ENGINE_VERSION(4, 22)
#define UE_4_22_OR_LATER 1
#else
#define UE_4_22_OR_LATER 0
#endif

/**
 * Categories used for log output with this plugin.
 */
GRAPHPRINTERGLOBALS_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphPrinter, Log, All);
