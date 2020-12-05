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

protected:
	// Internal processing of command binding.
	virtual void BindCommands();

private:
	// Called when the editor window is displayed.
	void HandleOnSlateWindowRendered(SWindow& SlateWindow, void* ViewportRHIPtr);

private:
	// Is the command registered here bound.
	bool bIsBound;

	// Delegate handle called when the editor window is displayed.
	FDelegateHandle SlateWindowRenderedHandle;

	// Instances of bound commands.
	TSharedPtr<FUICommandInfo> PrintGraphWithAllNodes;
	TSharedPtr<FUICommandInfo> PrintGraphWithSelectedNodes;
	TSharedPtr<FUICommandInfo> OpenExportDestinationFolder;
};
