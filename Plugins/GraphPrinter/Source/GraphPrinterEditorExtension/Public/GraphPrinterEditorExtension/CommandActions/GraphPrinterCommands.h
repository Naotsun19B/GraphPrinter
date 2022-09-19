// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

class FMenuBuilder;

namespace GraphPrinter
{
	/**
	 * Class to register the shortcut key used in this plugin.
	 */
	class GRAPHPRINTEREDITOREXTENSION_API FGraphPrinterCommands : public TCommands<FGraphPrinterCommands>
	{
	public:
		// Constructor.
		FGraphPrinterCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Is the command registered here bound.
		static bool IsBound();

		// Processing of command binding.
		static void Bind();

		// Fill the menu builder with the commands of this plugin.
		static void FillMenuBuilder(FMenuBuilder& MenuBuilder);

		// Returns commands with names matching the specified name.
		TSharedPtr<FUICommandInfo> FindCommandByName(const FName& CommandName) const;
		
	protected:
		// Internal processing of command binding.
		virtual void BindCommands();

	private:
		// Called when a submenu that opens plugin settings is built.
		static void OnExtendOpenSettingsSubMenu(FMenuBuilder& MenuBuilder);
		
	public:
		// A list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
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
		// Is the command registered here bound.
		bool bIsBound;
	};
}
