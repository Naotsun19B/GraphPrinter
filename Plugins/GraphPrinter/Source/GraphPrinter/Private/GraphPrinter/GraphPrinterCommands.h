// Copyright 2020-2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "GraphPrinter/GraphPrinterGlobals.h"

namespace GraphPrinter
{
	/**
	 * Class to register the shortcut key used in this plugin.
	 */
	class GRAPHPRINTER_API FGraphPrinterCommands : public TCommands<FGraphPrinterCommands>
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

	protected:
		// Internal processing of command binding.
		virtual void BindCommands();

	public:
		// Instances of bound commands.
#if ENABLE_IMAGE_TO_CLIPBOARD
		TSharedPtr<FUICommandInfo> CopyGraphWithAllNodesToClipboard;
		TSharedPtr<FUICommandInfo> CopyGraphWithSelectedNodesToClipboard;
#endif
		TSharedPtr<FUICommandInfo> PrintGraphWithAllNodes;
		TSharedPtr<FUICommandInfo> PrintGraphWithSelectedNodes;
#if ENABLE_EMBED_NODE_INFO
		TSharedPtr<FUICommandInfo> RestoreNodesFromPngFile;
#endif
		TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;

	private:
		// Is the command registered here bound.
		bool bIsBound;
	};
}
