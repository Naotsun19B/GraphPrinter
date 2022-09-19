// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterRemoteControl/WebSockets/GraphPrinterRemoteControlReceiver.h"
#include "GraphPrinterRemoteControl/Utilities/GraphPrinterRemoteControlSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommands.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

namespace GraphPrinter
{
	void FGraphPrinterRemoteControlReceiver::Register()
	{
		Instance = MakeUnique<FGraphPrinterRemoteControlReceiver>();
		check(Instance.IsValid());
		
		UGraphPrinterRemoteControlSettings::OnRemoteControlEnabled.AddRaw(
			Instance.Get(), &FGraphPrinterRemoteControlReceiver::ConnectToServer
		);
		UGraphPrinterRemoteControlSettings::OnRemoteControlDisabled.AddRaw(
			Instance.Get(), &FGraphPrinterRemoteControlReceiver::DisconnectFromServer
		);

		const auto& Settings = UGraphPrinterRemoteControlSettings::Get();
		if (Settings.bEnableRemoteControl)
		{
			Instance->ConnectToServer(Settings.ServerURL, Settings.ServerProtocol);
		}
	}

	void FGraphPrinterRemoteControlReceiver::Unregister()
	{
		UGraphPrinterRemoteControlSettings::OnRemoteControlEnabled.RemoveAll(Instance.Get());
		UGraphPrinterRemoteControlSettings::OnRemoteControlDisabled.RemoveAll(Instance.Get());

		Instance.Reset();
	}

	void FGraphPrinterRemoteControlReceiver::ConnectToServer(const FString ServerURL, const FString ServerProtocol)
	{
		DisconnectFromServer();
		
		Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
		check(Socket.IsValid());

		Socket->OnConnected().AddRaw(
			this, &FGraphPrinterRemoteControlReceiver::HandleOnConnected,
			ServerURL, ServerProtocol
		);
		Socket->OnConnectionError().AddRaw(this, &FGraphPrinterRemoteControlReceiver::HandleOnConnectionError);
		Socket->OnClosed().AddRaw(this, &FGraphPrinterRemoteControlReceiver::HandleOnClosed);
		Socket->OnMessage().AddRaw(this, &FGraphPrinterRemoteControlReceiver::HandleOnMessage);

		Socket->Connect();
	}

	void FGraphPrinterRemoteControlReceiver::DisconnectFromServer()
	{
		if (Socket.IsValid())
		{
			Socket->Close();
		}
		Socket.Reset();
	}

	void FGraphPrinterRemoteControlReceiver::HandleOnConnected(const FString ServerURL, const FString ServerProtocol)
	{
		UE_LOG(LogGraphPrinter, Log, TEXT("Connected to server (URL: %s | Protocol: %s)"), *ServerURL, *ServerProtocol);
	}

	void FGraphPrinterRemoteControlReceiver::HandleOnConnectionError(const FString& Error)
	{
		UE_LOG(LogGraphPrinter, Error, TEXT("Occurred connection error : %s"), *Error);
		UE_LOG(LogGraphPrinter, Error, TEXT("Make sure the server is up and re-enable remote control."));
	}

	void FGraphPrinterRemoteControlReceiver::HandleOnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
	{
		UE_LOG(LogGraphPrinter, Log, TEXT("Dissconnected from server (Status Code: %d | Reason: %s)"), StatusCode, *Reason);
	}

	void FGraphPrinterRemoteControlReceiver::HandleOnMessage(const FString& Message)
	{
		FName CommandName = NAME_None;
		{
			TArray<FString> ParsedMessage;
			Message.ParseIntoArray(ParsedMessage, TEXT("-"));
			if (ParsedMessage.Num() == 3)
			{
				CommandName = *ParsedMessage[2];
			}
		}
		
		const auto& Commands = FGraphPrinterCommands::Get();
		const TSharedPtr<FUICommandInfo>& CommandToExecute = Commands.FindCommandByName(CommandName);
		if (CommandToExecute.IsValid())
		{
			Commands.CommandBindings->ExecuteAction(CommandToExecute.ToSharedRef());
			UE_LOG(LogGraphPrinter, Log, TEXT("Received request from server : %s"), *CommandName.ToString());
		}
		else
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Received invalid message from server : %s"), *Message);
		}
	}

	TUniquePtr<FGraphPrinterRemoteControlReceiver> FGraphPrinterRemoteControlReceiver::Instance;
}
