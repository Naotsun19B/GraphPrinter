// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FToolBarBuilder;
class FExtender;
class SWidget;

namespace GraphPrinter
{
	/**
	 * A class that adds a menu that performs plugin functions to the toolbar of the Graph Editor.
	 */
	class FToolbarExtender
	{
	public:
		// Register-Unregister the toolbar extension.
		static void Register();
		static void Unregister();

		// Returns whether the toolbar should be extended.
		static bool ShouldExtendToolbar();
		
	private:
		// Called when the toolbar is extended.
		static void OnExtendToolbar(FToolBarBuilder& ToolBarBuilder);

		// Called when building the contents of a combo box added to the toolbar.
		static TSharedRef<SWidget> OnGetComboBoxContent();

	private:
		// Toolbar extension point.
		static TSharedPtr<FExtender> Extender;
	};
}
