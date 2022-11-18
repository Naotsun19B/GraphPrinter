// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/StreamDeckMenu.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformProcess.h"

#ifdef WITH_STREAM_DECK
#define LOCTEXT_NAMESPACE "StreamDeckMenu"

namespace GraphPrinter
{
	const FString FStreamDeckMenu::ApplicationName = TEXT("StreamDeck");
	
	void FStreamDeckMenu::FillMenuBuilder(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InstallPluginTitle", "Instal Stream Deck Plugin"),
			LOCTEXT("InstallPluginTooltip", "Install the Stream Deck plugin that works with this plugin on the Stream Deck.\nRequires the Stream Deck application to be running."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&FStreamDeckMenu::InstallStreamDeckPlugin),
				FCanExecuteAction::CreateStatic(&FStreamDeckMenu::IsStreamDeckRunning)
			),
			TEXT("Other")
		);
	}

	bool FStreamDeckMenu::IsStreamDeckRunning()
	{
		return FPlatformProcess::IsApplicationRunning(*ApplicationName);
	}

	void FStreamDeckMenu::InstallStreamDeckPlugin()
	{
		// #TODO: Execute the plugin file.
	}
}
	
#undef LOCTEXT_NAMESPACE
#endif
