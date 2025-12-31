// Copyright 2020-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SNotificationItem;

namespace GraphPrinter
{
	/**
	 * A struct of notification item handles that can indirectly manipulate SNotificationItem.
	 */
	struct GRAPHPRINTERGLOBALS_API FEditorNotificationHandle
	{
	public:
		// Constructor.
		FEditorNotificationHandle();
		explicit FEditorNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem);

		// Returns whether editor notifications pointed to by this handle are enabled.
		bool IsValid() const;

		// Changes the text of the currently displayed editor notification.
		void SetText(const FText& Text);

		// Begins the fade out,
		void Fadeout() const;
			
	private:
		// Indirectly operated SNotificationItem.
		TSharedPtr<SNotificationItem> NotificationItem;
	};
	
	/**
	 * A struct for adding buttons, hyperlinks, etc. to notifications.
	 */
	struct GRAPHPRINTERGLOBALS_API FEditorNotificationInteraction
	{
	public:
		// Types of things that users of editor notifications can interact with.
		enum EType
		{
			None,
			Hyperlink,
			Button,
		};
	
	public:
		// Constructor.
		FEditorNotificationInteraction();
		FEditorNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback);
		FEditorNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback);

		// Returns the types of things that users of editor notifications can interact with.
		EType GetType() const;

		// Returns the text displayed by the button or hyperlink.
		const FText& GetText() const;

		// Returns the tooltip text that appears when you hover over a button.
		const FText& GetTooltip() const;

		// Returns the callback that called when a button or hyperlink is pressed.
		const FSimpleDelegate& GetCallback()const ;

	private:
		// Constructor.
		FEditorNotificationInteraction(
			const EType InType,
			const FText& InText,
			const FText& InTooltip,
			const FSimpleDelegate& InCallback
		);

	private:
		// The types of things that users of editor notifications can interact with.
		EType Type;

		// The text displayed by the button or hyperlink.
		FText Text;

		// The tooltip text that appears when you hover over a button.
		FText Tooltip;

		// The callback that called when a button or hyperlink is pressed.
		FSimpleDelegate Callback;
	};
	
	/**
	 * A utility class for displaying editor notifications.
	 */
	class GRAPHPRINTERGLOBALS_API FEditorNotification
	{
	public:
		// Shows notifications at the bottom right of the editor (When Expire Duration is 0 or less, you need to call the fade process manually.).
		static FEditorNotificationHandle Pending(
			const FText& NotificationText,
			const float ExpireDuration = 4.f, 
			const TArray<FEditorNotificationInteraction>& Interactions = TArray<FEditorNotificationInteraction>()
		);
		static FEditorNotificationHandle Success(
			const FText& NotificationText,
			const float ExpireDuration = 4.f, 
			const TArray<FEditorNotificationInteraction>& Interactions = TArray<FEditorNotificationInteraction>()
		);
		static FEditorNotificationHandle Fail(
			const FText& NotificationText,
			const float ExpireDuration = 4.f, 
			const TArray<FEditorNotificationInteraction>& Interactions = TArray<FEditorNotificationInteraction>()
		);
	};
}