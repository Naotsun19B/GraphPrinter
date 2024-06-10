// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MaterialGraphExportMethod.generated.h"

/**
 * An enum that defines how to output a graph in the material editor.
 */
UENUM()
enum class EMaterialGraphExportMethod : uint8
{
	GraphOnly,
	CombinePreviewAndGraph,
	PreviewAndGraphSeparately,
};
