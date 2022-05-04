// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphPrinterEditorExtension
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

	private:
		// Called when the toolbar is expanded.
		static void OnExtendToolbar(FToolBarBuilder& Builder);

		// Called when building the contents of a combo box added to the toolbar.
		static TSharedRef<SWidget> OnGetComboBoxContent();

	private:
		// Toolbar extension point.
		static TSharedPtr<FExtender> Extender;
	};
}
