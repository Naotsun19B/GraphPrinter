// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GenericGraphPrinter/WidgetPrinters/InnerGenericGraphPrinter.h"

namespace GraphPrinter
{
#ifdef WITH_TEXT_CHUNK_HELPER
	const FString FGenericGraphPrinter::PngTextChunkKey = TEXT("GraphEditor");
	const FString FGenericGraphPrinter::NodeInfoHeader = TEXT("Begin Object");
#endif
}
