// Fill out your copyright notice in the Description page of Project Settings.

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
 * Categories used for log output with this plugin.
 */
GRAPHPRINTER_API DECLARE_LOG_CATEGORY_EXTERN(LogGraphPrinter, Log, All);
