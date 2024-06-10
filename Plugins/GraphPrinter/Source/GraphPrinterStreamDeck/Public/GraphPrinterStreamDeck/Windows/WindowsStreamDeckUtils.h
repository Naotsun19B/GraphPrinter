// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterStreamDeck/GenericPlatform/GenericStreamDeckUtils.h"

namespace GraphPrinter
{
	/**
	 * A class that defines the processing related to Stream Deck on Windows.
	 */
	class GRAPHPRINTERSTREAMDECK_API FWindowsStreamDeckUtils : public FGenericStreamDeckUtils
	{
	public:
		// FGenericStreamDeckUtils interface.
		static bool IsStreamDeckInstalled();
		static void InstallStreamDeckPlugin();
		static bool IsInstalledStreamDeckPlugin();
		// End of FGenericStreamDeckUtils interface.
	};

#if PLATFORM_WINDOWS
	using FStreamDeckUtils = FWindowsStreamDeckUtils;
#endif
}
