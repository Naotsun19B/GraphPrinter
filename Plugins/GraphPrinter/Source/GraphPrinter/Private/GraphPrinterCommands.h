// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class GRAPHPRINTER_API GraphPrinterCommands : public TCommands<GraphPrinterCommands>
{
public:
	// Constructor.
	GraphPrinterCommands();

	// TCommands interface.
	virtual void RegisterCommands() override;
	// End of TCommands interface.

	// Bind the registered UI Command.
	static void Bind();

protected:
	// Internal processing of command binding.
	virtual void BindCommands() const;

private:
	// A command to export the currently active graph editor as an image.
	TSharedPtr<FUICommandInfo> PrintGraph;
};
