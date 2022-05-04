// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

namespace GraphPrinterEditorExtension
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
		
	protected:
		// Internal processing of command binding.
		virtual void BindCommands();

	public:
		// A list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
		TSharedPtr<FUICommandInfo> CopyGraphWithAllNodesToClipboard;
		TSharedPtr<FUICommandInfo> CopyGraphWithSelectedNodesToClipboard;
#endif
		TSharedPtr<FUICommandInfo> PrintGraphWithAllNodes;
		TSharedPtr<FUICommandInfo> PrintGraphWithSelectedNodes;
#ifdef WITH_TEXT_CHUNK_HELPER
		TSharedPtr<FUICommandInfo> RestoreNodesFromPngFile;
#endif
		TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;

	private:
		// Is the command registered here bound.
		bool bIsBound;
	};
}
