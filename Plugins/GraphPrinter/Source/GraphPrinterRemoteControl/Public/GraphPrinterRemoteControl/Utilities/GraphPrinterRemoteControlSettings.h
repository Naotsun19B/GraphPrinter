// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterRemoteControlSettings.generated.h"

/**
 * Settings related to the remote control of this plugin.
 */
UCLASS()
class UGraphPrinterRemoteControlSettings : public UGraphPrinterSettings
{
	GENERATED_BODY()

public:
	// Whether remote control via web socket is enabled.
	// Please check again when the server is rebuilt.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control")
	bool bEnableRemoteControl;
	
	// Your server URL. You can use ws, wss or wss+insecure.
	// Disable remote control once to edit.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control", meta = (EditCondition = "!bEnableRemoteControl"))
	FString ServerURL;
	
	// The WebServer protocol you want to use.
	// Disable remote control once to edit.
	UPROPERTY(EditAnywhere, Config, Category = "Remote Control", meta = (EditCondition = "!bEnableRemoteControl"))
	FString ServerProtocol;

public:
	// The event called when remote control is enabled.
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRemoteControlEnabled, const FString /* ServerURL */, const FString /* ServerProtocol */);
	static FOnRemoteControlEnabled OnRemoteControlEnabled;

	// The event called when remote control is disabled.
	DECLARE_MULTICAST_DELEGATE(FOnRemoteControlDisabled);
	static FOnRemoteControlDisabled OnRemoteControlDisabled;
	
public:
	// Constructor.
	UGraphPrinterRemoteControlSettings();

	// Returns reference of this settings.
	static const UGraphPrinterRemoteControlSettings& Get();

	// UObject interface.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
	
	// UGraphPrinterSettings interface.
	virtual FSettingsInfo GetSettingsInfo() const override;
	// End of UGraphPrinterSettings interface.
};
