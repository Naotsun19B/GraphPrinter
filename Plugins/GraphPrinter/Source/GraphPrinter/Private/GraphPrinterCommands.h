// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class GraphPrinterCommands : public TCommands<GraphPrinterCommands>
{
public:
	// Constructor.
	GraphPrinterCommands();

	// TCommands interface.
	virtual void RegisterCommands() override;
	// End of TCommands interface.

public:
	// A command to export the currently active graph editor as an image.
	TSharedPtr<FUICommandInfo> PrintGraph;
};
