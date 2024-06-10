// Copyright 2020-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ImageWriteTypes.h"

class SNotificationItem;

namespace GraphPrinter
{
	// A struct of notification item handles that can indirectly manipulate SNotificationItem.
	struct GRAPHPRINTERGLOBALS_API FNotificationHandle
	{
	public:
		// Constructor.
		explicit FNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem);

		// Begin the fade out,
		void Fadeout();
			
	private:
		// Indirectly operated SNotificationItem.
		TSharedPtr<SNotificationItem> NotificationItem;
	};
		
	// A struct for adding buttons, hyperlinks, etc. to notifications.
	struct GRAPHPRINTERGLOBALS_API FNotificationInteraction
	{
	public:
		// Types of things that users of editor notifications can interact with.
		enum class EInteractionType : uint8 
		{
			Hyperlink,
			Button,
		};
		EInteractionType Type;

		// The text displayed by the button or hyperlink.
		FText Text;

		// Tooltip text that appears when you hover over a button.
		FText Tooltip;

		// A callback that is called when a button or hyperlink is pressed.
		FSimpleDelegate Callback;

	public:
		// Constructor.
		FNotificationInteraction(
			const EInteractionType InType,
			const FText& InText,
			const FText& InTooltip,
			const FSimpleDelegate& InCallback
		);
		FNotificationInteraction();
		FNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback);
		FNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback);
	};
	
	/**
	 * A utility class that defines utility functions used in this plugin.
	 */
	class GRAPHPRINTERGLOBALS_API FGraphPrinterUtils
	{
	public:
		// Define notification types so don't need to include "SNotificationList.h".
		using ECompletionState = int32;
		static const ECompletionState CS_Pending;
		static const ECompletionState CS_Success;
		static const ECompletionState CS_Fail;
		
	public:
		// Shows notifications at the bottom right of the editor (When Expire Duration is 0 or less, you need to call the fade process manually.).
		static FNotificationHandle ShowNotification(
			const FText& NotificationText, 
			ECompletionState CompletionState, 
			float ExpireDuration = 4.f, 
			const TArray<FNotificationInteraction>& Interactions = TArray<FNotificationInteraction>()
		);

		// Gets the extension by the format of the image file.
		static FString GetImageFileExtension(const EDesiredImageFormat ImageFormat, const bool bWithDot = true);
		
		// Opens the folder containing the file in Explorer.
		static void OpenFolderWithExplorer(const FString& Filename);

		// Shows the file browser and let the user select a file.
		static bool OpenFileDialog(
			TArray<FString>& Filenames,
			const FString& DialogTitle = TEXT("Open File Dialog"),
			const FString& DefaultPath = TEXT(""),
			const FString& DefaultFile = TEXT(""),
			const FString& FileTypes = TEXT("All (*)|*.*"),
			const bool bIsMultiple = false
		);

		// Cuts out unnecessary character strings mixed in at the beginning of sentences.
		// Returns whether the clipping was actually done.
		static bool ClearUnnecessaryCharactersFromHead(FString& String, const FString& BeginningOfString);
	};
}
