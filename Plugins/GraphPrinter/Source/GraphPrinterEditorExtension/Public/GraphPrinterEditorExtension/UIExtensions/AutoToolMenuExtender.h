// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ToolMenus.h"

struct FDelayedAutoRegisterHelper;

namespace GraphPrinter
{
	/**
	 * A base class that extend the menu on the editor after the engine initialization is complete.
	 */
	class GRAPHPRINTEREDITOREXTENSION_API FAutoToolMenuExtender
	{
	public:
		// Destructor.
		virtual ~FAutoToolMenuExtender() = default;

	protected:
		// Constructor.
		FAutoToolMenuExtender();
		
		// Actually expands the editor menu.
		virtual void ExtendToolMenu() = 0;

	private:
		// The instance of the helper that actually calls the menu expansion at a delayed time.
		TOptional<FDelayedAutoRegisterHelper> DelayedAutoRegister;
	};
}
