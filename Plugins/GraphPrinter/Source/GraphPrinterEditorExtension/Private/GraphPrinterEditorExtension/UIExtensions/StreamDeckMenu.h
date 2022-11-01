// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#ifdef WITH_STREAM_DECK
class FMenuBuilder;

namespace GraphPrinter
{
	/**
	 * A class that adds items to the tool menu for Stream Deck operations.
	 */
	class FStreamDeckMenu
	{
	public:
		// The process name of the Stream Deck application.
		static const FString ApplicationName;
		
	public:
		// Adds a tool menu related to stream decks.
		static void FillMenuBuilder(FMenuBuilder& MenuBuilder);

		// Returns whether the Stream Deck application is running.
		static bool IsStreamDeckRunning();

	private:
		// Install the Stream Deck plugin that works with this plugin on the Stream Deck.
		static void InstallStreamDeckPlugin();
	};
}
#endif
