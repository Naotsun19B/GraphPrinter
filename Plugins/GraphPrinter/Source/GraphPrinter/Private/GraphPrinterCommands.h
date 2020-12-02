// Fill out your copyright notice in the Description page of Project Settings.

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

	// Bind the registered UI Command.
	static void Bind();

protected:
	// Internal processing of command binding.
	virtual void BindCommands() const;

private:
	// Instances of bound commands.
	TSharedPtr<FUICommandInfo> PrintGraphWithAllNodes;
	TSharedPtr<FUICommandInfo> PrintGraphWithSelectedNodes;
	TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;
};
