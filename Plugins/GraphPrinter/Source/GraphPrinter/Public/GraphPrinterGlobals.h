// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/Launch/Resources/Version.h"

/**
 * Macro to support each engine version.
 */
#if ENGINE_MINOR_VERSION <= 23
#define BEFORE_UE_4_23 1
#else
#define BEFORE_UE_4_23 0
#endif

#if ENGINE_MINOR_VERSION <= 21
#define BEFORE_UE_4_21 1
#else
#define BEFORE_UE_4_21 0
#endif

/**
 * Defines whether the ability to embed node information in text chunks is available
 */
#ifdef WITH_EMBED_NODE_INFO
#define ENABLE_EMBED_NODE_INFO WITH_UNREALPNG
#else
#define ENABLE_EMBED_NODE_INFO 0
#endif

/**
 * Categories used for log output with this plugin.
 */
GRAPHPRINTER_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphPrinter, Log, All);
