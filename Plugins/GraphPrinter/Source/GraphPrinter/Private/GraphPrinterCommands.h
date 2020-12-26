// Copyright 2020 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

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
	TSharedPtr<FUICommandInfo> PrintGraphWithAllNodes;
	TSharedPtr<FUICommandInfo> PrintGraphWithSelectedNodes;
	TSharedPtr<FUICommandInfo> RestoreNodesFromPngFile;
	TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;

private:
	// Is the command registered here bound.
	bool bIsBound;
};
