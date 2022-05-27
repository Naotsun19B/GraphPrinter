// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FMenuBuilder;
class FExtender;

namespace GraphPrinter
{
	/**
	 * A class that adds a menu that performs plugin functions to the tool menu of the Graph Editor.
	 */
	class FToolMenuExtender
	{
	public:
		// Register-Unregister the toolbar extension.
		static void Register();
		static void Unregister();

	private:
		// Called when the tool menu is expanded.
		static void OnExtendToolMenu(FMenuBuilder& MenuBuilder);

		// Called when an extended submenu is built.
		static void OnExtendSubMenu(FMenuBuilder& MenuBuilder);

	private:
		// Toolbar extension point.
		static TSharedPtr<FExtender> Extender;
	};
}
