// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

class UToolMenu;

namespace GraphPrinter
{
	/**
	 * A class to register the shortcut key used in this plugin.
	 */
	class GRAPHPRINTEREDITOREXTENSION_API FGraphPrinterCommands : public TCommands<FGraphPrinterCommands>
	{
	public:
		// Constructor.
		FGraphPrinterCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Returns whether the commands registered here are bound.
		static bool IsBound();

		// Binds commands registered here.
		static void Bind();

		// Fills the menu builder with the commands of this plugin.
		static void FillMenuBuilder(UToolMenu* ToolMenu);

		// Returns commands with names matching the specified name.
		TSharedPtr<FUICommandInfo> FindCommandByName(const FName& CommandName) const;
		
	protected:
		// Binds commands actually registered here.
		virtual void BindCommands();

	private:
		// Called when a submenu that opens plugin settings is built.
		static void OnExtendOpenSettingsSubMenu(UToolMenu* ToolMenu);
		
	public:
		// The list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> CollectTargetWidgets;
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		TSharedPtr<FUICommandInfo> CopyAllAreaOfWidgetToClipboard;
		TSharedPtr<FUICommandInfo> CopySelectedAreaOfWidgetToClipboard;
#endif
		TSharedPtr<FUICommandInfo> PrintAllAreaOfWidget;
		TSharedPtr<FUICommandInfo> PrintSelectedAreaOfWidget;
#ifdef WITH_TEXT_CHUNK_HELPER
		TSharedPtr<FUICommandInfo> RestoreWidgetFromImageFile;
#endif
		TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;

	private:
		// Whether the commands registered here are bound.
		bool bIsBound;
	};
}
