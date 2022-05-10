// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MaterialGraphExportMethod.generated.h"

/**
 * How to output a graph in the material editor.
 */
UENUM()
enum class EMaterialGraphExportMethod : uint8
{
	GraphOnly,
	CombinePreviewAndGraph,
	PreviewAndGraphSeparately,
};
