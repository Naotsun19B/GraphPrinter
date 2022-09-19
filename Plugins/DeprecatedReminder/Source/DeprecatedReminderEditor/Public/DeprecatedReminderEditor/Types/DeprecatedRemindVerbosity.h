// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * An enum that defines what to do with expired elements.
 */
UENUM()
enum class EDeprecatedRemindVerbosity : uint8
{
	// You can check it from the editor, but nothing is done at cook.
	DoNothing,
	// Log is output when the editor is started.
	LogAtStartEditor,
	// Issue a warning when cook. This will not cause the cook process to fail.
	WarningAtCook,
	// Raise an error when cook. This will cause the cook process to fail.
	ErrorAtCook,
};

/**
 * An enum that defines what to do at compile time for expired code.
 */
UENUM()
enum class EDeprecatedRemindVerbosityAtCompileTime : uint8
{
	// You can check it from the editor, but nothing is done at compile time.
	DoNothing,
	// Issue a warning at compile time. This will not cause the compilation to fail.
	Warning,
	// Raise an error at compile time. This will cause the compilation to fail.
	Error,
};
