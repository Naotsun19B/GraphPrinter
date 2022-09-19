// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterRemoteControl/Utilities/GraphPrinterRemoteControlSettings.h"

UGraphPrinterRemoteControlSettings::FOnRemoteControlEnabled UGraphPrinterRemoteControlSettings::OnRemoteControlEnabled;
UGraphPrinterRemoteControlSettings::FOnRemoteControlDisabled UGraphPrinterRemoteControlSettings::OnRemoteControlDisabled;

UGraphPrinterRemoteControlSettings::UGraphPrinterRemoteControlSettings()
	: bEnableRemoteControl(false)
	, ServerURL(TEXT("ws://127.0.0.1:3000/"))
	, ServerProtocol(TEXT("ws"))
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
			OnRemoteControlEnabled.Broadcast(ServerURL, ServerProtocol);
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
