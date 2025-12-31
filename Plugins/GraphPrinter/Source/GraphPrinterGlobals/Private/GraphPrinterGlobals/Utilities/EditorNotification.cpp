// Copyright 2020-2026 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/EditorNotification.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace GraphPrinter
{
	FEditorNotificationHandle::FEditorNotificationHandle()
		: NotificationItem(nullptr)
	{
	}

	FEditorNotificationHandle::FEditorNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem)
		: NotificationItem(InNotificationItem)
	{
	}

	bool FEditorNotificationHandle::IsValid() const
	{
		return NotificationItem.IsValid();
	}

	void FEditorNotificationHandle::SetText(const FText& Text)
	{
		if (IsValid())
		{
			NotificationItem->SetText(Text);
		}
	}

	void FEditorNotificationHandle::Fadeout() const
	{
		if (IsValid())
		{
			NotificationItem->Fadeout();
		}
	}
	
	FEditorNotificationInteraction::FEditorNotificationInteraction()
		: FEditorNotificationInteraction(None, {}, {}, {})
	{
	}

	FEditorNotificationInteraction::FEditorNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback)
		: FEditorNotificationInteraction(Hyperlink, InText, {}, InCallback)
	{
	}

	FEditorNotificationInteraction::FEditorNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback)
		: FEditorNotificationInteraction(Button, InText, InTooltip, InCallback)
	{
	}

	FEditorNotificationInteraction::EType FEditorNotificationInteraction::GetType() const
	{
		return Type;
	}

	const FText& FEditorNotificationInteraction::GetText() const
	{
		return Text;
	}

	const FText& FEditorNotificationInteraction::GetTooltip() const
	{
		return Tooltip;
	}

	const FSimpleDelegate& FEditorNotificationInteraction::GetCallback() const
	{
		return Callback;
	}

	FEditorNotificationInteraction::FEditorNotificationInteraction(
		const EType InType,
		const FText& InText,
		const FText& InTooltip,
		const FSimpleDelegate& InCallback
	)
		: Type(InType), Text(InText), Tooltip(InTooltip), Callback(InCallback)
	{
	}
	
	namespace EditorNotification
	{
		static FEditorNotificationHandle Post(
			const FText& NotificationText, 
			const SNotificationItem::ECompletionState CompletionState, 
			const float ExpireDuration, 
			const TArray<FEditorNotificationInteraction>& Interactions
		)
		{
			FNotificationInfo NotificationInfo(NotificationText);
			NotificationInfo.bFireAndForget = (ExpireDuration > 0.f);
			if (NotificationInfo.bFireAndForget)
			{
				NotificationInfo.ExpireDuration = ExpireDuration;
			}
			NotificationInfo.bUseLargeFont = true;
			
			for (const auto& Interaction : Interactions)
			{
				switch (Interaction.GetType())
				{
				case FEditorNotificationInteraction::Hyperlink:
					{
						NotificationInfo.HyperlinkText = Interaction.GetText();
						NotificationInfo.Hyperlink = Interaction.GetCallback();
						break;
					}
				case FEditorNotificationInteraction::Button:
					{
						const FNotificationButtonInfo ButtonInfo(
							Interaction.GetText(),
							Interaction.GetTooltip(),
							Interaction.GetCallback(),
							CompletionState
						);
						NotificationInfo.ButtonDetails.Add(ButtonInfo);
						break;
					}
				default:
					break;
				}
			}

			const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
			if (NotificationItem.IsValid())
			{
				NotificationItem->SetCompletionState(CompletionState);
			}
	
			return FEditorNotificationHandle(NotificationItem);
		}
	}

	FEditorNotificationHandle FEditorNotification::Pending(
		const FText& NotificationText,
		const float ExpireDuration /* = 4.f */,
		const TArray<FEditorNotificationInteraction>& Interactions /* = TArray<FNotificationInteraction>() */
	)
	{
		return EditorNotification::Post(NotificationText, SNotificationItem::ECompletionState::CS_Pending, ExpireDuration, Interactions);
	}

	FEditorNotificationHandle FEditorNotification::Success(
		const FText& NotificationText,
		const float ExpireDuration /* = 4.f */,
		const TArray<FEditorNotificationInteraction>& Interactions /* = TArray<FNotificationInteraction>() */
	)
	{
		return EditorNotification::Post(NotificationText, SNotificationItem::ECompletionState::CS_Success, ExpireDuration, Interactions);
	}

	FEditorNotificationHandle FEditorNotification::Fail(
		const FText& NotificationText,
		const float ExpireDuration /* = 4.f */,
		const TArray<FEditorNotificationInteraction>& Interactions /* = TArray<FNotificationInteraction>() */
	)
	{
		return EditorNotification::Post(NotificationText, SNotificationItem::ECompletionState::CS_Fail, ExpireDuration, Interactions);
	}
}
