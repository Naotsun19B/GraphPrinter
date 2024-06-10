// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/UIExtensions/AutoToolMenuExtender.h"
#include "Misc/DelayedAutoRegister.h"

namespace GraphPrinter
{
	FAutoToolMenuExtender::FAutoToolMenuExtender()
	{
		DelayedAutoRegister = FDelayedAutoRegisterHelper(
			EDelayedRegisterRunPhase::EndOfEngineInit,
			[this]()
			{
				UToolMenus::RegisterStartupCallback(
					FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAutoToolMenuExtender::ExtendToolMenu)
				);
			}
		);
	}
}
