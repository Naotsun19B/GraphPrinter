// Copyright 2020-2023 Naotsun. All Rights Reserved.

#include "GraphPrinterStreamDeck/UIExtensions/StreamDeckMenu.h"
#include "GraphPrinterStreamDeck/HAL/StreamDeckUtils.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#ifdef WITH_STREAM_DECK
#define LOCTEXT_NAMESPACE "StreamDeckMenu"

namespace GraphPrinter
{
	void FStreamDeckMenu::FillMenuBuilder(FMenuBuilder& MenuBuilder)
	{
		if (!FStreamDeckUtils::IsStreamDeckInstalled())
		{
			return;
		}
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InstallPluginTitle", "Install Stream Deck Plugin"),
			LOCTEXT("InstallPluginTooltip", "Install the Stream Deck plugin that works with this plugin on the Stream Deck.\nIf this item fails to run, it is already installed.\nIf you don't see the Graph Printer item in the Stream Deck application, restart the Stream Deck application."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&FStreamDeckUtils::InstallStreamDeckPlugin),
				FCanExecuteAction::CreateLambda(
					[]() -> bool
					{
						return !FStreamDeckUtils::IsInstalledStreamDeckPlugin();
					}
				)
			),
			TEXT("Other")
		);
	}
}
	
#undef LOCTEXT_NAMESPACE
#endif
