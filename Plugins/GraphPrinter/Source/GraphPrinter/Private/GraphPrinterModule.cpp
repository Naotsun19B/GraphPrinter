// Copyright 2020 Naotsun. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterCommands.h"
#include "GraphPrinterSettings.h"
#include "GraphPrinterCore.h"

DEFINE_LOG_CATEGORY(LogGraphPrinter);

class FGraphPrinterModule : public IModuleInterface
{
public:
	// IModuleInterface interface.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface.

private:
	// Called when the editor window is displayed.
	void HandleOnSlateWindowRendered(SWindow& SlateWindow, void* ViewportRHIPtr);

private:
	// Delegate handle called when the editor window is displayed.
	FDelegateHandle SlateWindowRenderedHandle;
};

#define LOCTEXT_NAMESPACE "GraphPrinter"

void FGraphPrinterModule::StartupModule()
{
	UGraphPrinterSettings::Register();
	FGraphPrinterCommands::Register();
	FGraphPrinterCommands::Bind();

	// The image that is output for the first time after startup may be whitish, 
	// so as a countermeasure, render it here once.
	if (FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer())
	{
		SlateWindowRenderedHandle = SlateRenderer->OnSlateWindowRendered().AddRaw(this, &FGraphPrinterModule::HandleOnSlateWindowRendered);
	}
}

void FGraphPrinterModule::ShutdownModule()
{
	FGraphPrinterCommands::Unregister();
	UGraphPrinterSettings::Unregister();
}

void FGraphPrinterModule::HandleOnSlateWindowRendered(SWindow& SlateWindow, void* ViewportRHIPtr)
{
	TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (ActiveWindow.IsValid())
	{
		FGraphPrinterCore::DrawWidgetToRenderTarget(ActiveWindow, FVector2D(1920, 1080), true, TF_Default);
	}

	if (FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer())
	{
		SlateRenderer->OnSlateWindowRendered().Remove(SlateWindowRenderedHandle);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGraphPrinterModule, GraphPrinter)
