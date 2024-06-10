// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IWebSocket;

namespace GraphPrinter
{
	/**
	 * A receiver class that utilizes the functionality of this plugin externally via a web socket.
	 * 
	 * The format of the request from the server is as follows:
	 * UnrealEngine-GraphPrinter-[CommandName]
	 * 
	 * CommandName is the name of the command defined in FGraphPrinterCommands.
	 */
	class GRAPHPRINTERREMOTECONTROL_API FGraphPrinterRemoteControlReceiver
	{
	public:
		// Registers-Unregisters the remote control receiver.
		static void Register();
		static void Unregister();

	private:
		// Called when remote control is enabled.
		void ConnectToServer(const FString ServerURL);

		// Called when remote control is disabled.
		void DisconnectFromServer();

		// Callback functions for events emitted from web sockets.
		void HandleOnConnected(const FString ServerURL);
		void HandleOnConnectionError(const FString& Error);
		void HandleOnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
		void HandleOnMessage(const FString& Message);
		
	private:
		// The currently connected web socket instance.
		TSharedPtr<IWebSocket> Socket;

		// The unique instance of this class.
		static TUniquePtr<FGraphPrinterRemoteControlReceiver> Instance;
	};
}
