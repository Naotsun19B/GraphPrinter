// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Version.h"

namespace GraphPrinter
{
	namespace Global
	{
		// The name of this plugin.
		static const FName PluginName = TEXT("GraphPrinter");
	}
}

/**
 * Macros to support each engine version.
 */
#ifndef ENGINE_VERSION_NUMBER
#define ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion) (MajorVersion * 100 + MinorVersion)
#endif
#ifndef COMPARE_ENGINE_VERSION
#define COMPARE_ENGINE_VERSION(MajorVersion, MinorVersion) ENGINE_VERSION_NUMBER(ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION) >= ENGINE_VERSION_NUMBER(MajorVersion, MinorVersion)
#endif

#ifndef UE_5_02_OR_LATER
#if COMPARE_ENGINE_VERSION(5, 2)
#define UE_5_02_OR_LATER 1
#else
#define UE_5_02_OR_LATER 0
#endif
#endif

#ifndef UE_5_01_OR_LATER
#if COMPARE_ENGINE_VERSION(5, 1)
#define UE_5_01_OR_LATER 1
#else
#define UE_5_01_OR_LATER 0
#endif
#endif

#ifndef UE_5_00_OR_LATER
#if COMPARE_ENGINE_VERSION(5, 0)
#define UE_5_00_OR_LATER 1
#else
#define UE_5_00_OR_LATER 0
#endif
#endif

#ifndef UE_4_25_OR_LATER
#if COMPARE_ENGINE_VERSION(4, 25)
#define UE_4_25_OR_LATER 1
#else
#define UE_4_25_OR_LATER 0
#endif
#endif

#ifndef UE_4_24_OR_LATER
#if COMPARE_ENGINE_VERSION(4, 24)
#define UE_4_24_OR_LATER 1
#else
#define UE_4_24_OR_LATER 0
#endif
#endif

#ifndef UE_4_22_OR_LATER
#if COMPARE_ENGINE_VERSION(4, 22)
#define UE_4_22_OR_LATER 1
#else
#define UE_4_22_OR_LATER 0
#endif
#endif

/**
 * Categories used for log output with this plugin.
 */
GRAPHPRINTERGLOBALS_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphPrinter, Log, All);
