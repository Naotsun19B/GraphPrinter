// Copyright 2020-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#ifdef WITH_STREAM_DECK
class FMenuBuilder;

namespace GraphPrinter
{
	/**
	 * A class that adds items to the tool menu for Stream Deck operations.
	 */
	class GRAPHPRINTERSTREAMDECK_API FStreamDeckMenu
	{
	public:
		// Adds a tool menu related to stream decks.
		static void FillMenuBuilder(FMenuBuilder& MenuBuilder);
	};
}
#endif
