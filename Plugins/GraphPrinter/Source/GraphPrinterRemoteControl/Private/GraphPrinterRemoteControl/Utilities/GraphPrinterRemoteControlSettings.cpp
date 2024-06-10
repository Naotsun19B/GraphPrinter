// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "GraphPrinterRemoteControl/Utilities/GraphPrinterRemoteControlSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(GraphPrinterRemoteControlSettings)
#endif

UGraphPrinterRemoteControlSettings::FOnRemoteControlEnabled UGraphPrinterRemoteControlSettings::OnRemoteControlEnabled;
UGraphPrinterRemoteControlSettings::FOnRemoteControlDisabled UGraphPrinterRemoteControlSettings::OnRemoteControlDisabled;

UGraphPrinterRemoteControlSettings::UGraphPrinterRemoteControlSettings()
	: bEnableRemoteControl(false)
	, ServerURL(TEXT("ws://127.0.0.1:3000/"))
{
}

const UGraphPrinterRemoteControlSettings& UGraphPrinterRemoteControlSettings::Get()
{
	const auto* Settings = GetDefault<UGraphPrinterRemoteControlSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UGraphPrinterRemoteControlSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterRemoteControlSettings, bEnableRemoteControl))
	{
		if (bEnableRemoteControl)
		{
			OnRemoteControlEnabled.Broadcast(ServerURL);
		}
		else
		{
			OnRemoteControlDisabled.Broadcast();
		}
	}
}

UGraphPrinterSettings::FSettingsInfo UGraphPrinterRemoteControlSettings::GetSettingsInfo() const
{
	return FSettingsInfo(TEXT("RemoteControl"));
}
