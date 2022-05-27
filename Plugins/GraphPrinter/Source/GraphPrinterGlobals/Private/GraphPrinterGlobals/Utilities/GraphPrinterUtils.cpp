// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IMainFrameModule.h"
#include "DesktopPlatformModule.h"

namespace GraphPrinter
{
	FNotificationHandle::FNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem)
		: NotificationItem(InNotificationItem)
	{
	}

	void FNotificationHandle::Fadeout()
	{
		if (NotificationItem.IsValid())
		{
			NotificationItem->Fadeout();
		}
	}

	void FNotificationHandle::Pulse(const FLinearColor& GlowColor)
	{
		if (NotificationItem.IsValid())
		{
			NotificationItem->Pulse(GlowColor);
		}
	}

	FNotificationInteraction::FNotificationInteraction(
		EInteractionType InType,
		const FText& InText,
		const FText& InTooltip,
		const FSimpleDelegate& InCallback
	)
		: Type(InType), Text(InText), Tooltip(InTooltip), Callback(InCallback)
	{
	}
	
	FNotificationInteraction::FNotificationInteraction()
		: FNotificationInteraction(EInteractionType::Hyperlink, {}, {}, {})
	{
	}

	FNotificationInteraction::FNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback)
		: FNotificationInteraction(EInteractionType::Hyperlink, InText, {}, InCallback)
	{
	}

	FNotificationInteraction::FNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback)
		: FNotificationInteraction(EInteractionType::Button, InText, InTooltip, InCallback)
	{
	}
	
	constexpr FGraphPrinterUtils::ECompletionState FGraphPrinterUtils::CS_Pending	= SNotificationItem::ECompletionState::CS_Pending;
	constexpr FGraphPrinterUtils::ECompletionState FGraphPrinterUtils::CS_Success	= SNotificationItem::ECompletionState::CS_Success;
	constexpr FGraphPrinterUtils::ECompletionState FGraphPrinterUtils::CS_Fail		= SNotificationItem::ECompletionState::CS_Fail;
	
	FNotificationHandle FGraphPrinterUtils::ShowNotification(
		const FText& NotificationText,
		ECompletionState CompletionState,
		const float ExpireDuration /* = 4.f */,
		const TArray<FNotificationInteraction>& Interactions /* = TArray<FNotificationInteraction>() */
	)
	{
		FNotificationInfo NotificationInfo(NotificationText);
		NotificationInfo.bFireAndForget = (ExpireDuration > 0.f);
		if (NotificationInfo.bFireAndForget)
		{
			NotificationInfo.ExpireDuration = ExpireDuration;
		}
		NotificationInfo.bUseLargeFont = true;

		const auto StateEnum = static_cast<SNotificationItem::ECompletionState>(CompletionState);

		for (const auto& Interaction : Interactions)
		{
			switch (Interaction.Type)
			{
			case FNotificationInteraction::EInteractionType::Hyperlink:
				{
					NotificationInfo.HyperlinkText = Interaction.Text;
					NotificationInfo.Hyperlink = Interaction.Callback;
					break;
				}
			case FNotificationInteraction::EInteractionType::Button:
				{
					const FNotificationButtonInfo ButtonInfo(Interaction.Text, Interaction.Tooltip, Interaction.Callback, StateEnum);
					NotificationInfo.ButtonDetails.Add(ButtonInfo);
					break;
				}
			default: break;
			}
		}

		const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetCompletionState(StateEnum);
		}
	
		return FNotificationHandle(NotificationItem);
	}

	FString FGraphPrinterUtils::GetImageFileExtension(const EDesiredImageFormat ImageFormat, const bool bWithDot /* = true */)
	{
		FString Dot;
		if (bWithDot)
		{
			Dot = TEXT(".");
		}
		
#if BEFORE_UE_4_21
		if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDesiredImageFormat"), true))
		{
			const FString& Extension = EnumPtr->GetNameStringByIndex(static_cast<int32>(ImageFormat));
			return (Dot + FString::Printf(TEXT("%s"), *Extension.ToLower()));
		}
#else
		if (const UEnum* EnumPtr = StaticEnum<EDesiredImageFormat>())
		{
			const FString& EnumString = EnumPtr->GetValueAsString(ImageFormat);
			FString UnusedString;
			FString Extension;
			if (EnumString.Split(TEXT("::"), &UnusedString, &Extension))
			{
				Extension = Extension.ToLower();
				return (Dot + FString::Printf(TEXT("%s"), *Extension));
			}
		}
#endif
	
		checkNoEntry();
		return {};
	}
	
	void FGraphPrinterUtils::OpenFolderWithExplorer(const FString& Filename)
	{
		const FString& FullFilename = FPaths::ConvertRelativePathToFull(Filename);

		FText ValidatePathErrorText;
		if (!FPaths::ValidatePath(FullFilename, &ValidatePathErrorText))
		{
			ShowNotification(ValidatePathErrorText, CS_Fail);
			return;
		}

		FPlatformProcess::ExploreFolder(*FullFilename);
	}

	bool FGraphPrinterUtils::OpenFileDialog(
		TArray<FString>& Filenames, 
		const FString& DialogTitle /* = TEXT("Open File Dialog") */,
		const FString& DefaultPath /* = TEXT("") */,
		const FString& DefaultFile /* = TEXT("") */,
		const FString& FileTypes /* = TEXT("All (*)|*.*") */, 
		bool bIsMultiple /* = false */
	)
	{
		// Get the OS-level window handle of the editor's mainframe.
		void* WindowHandle = nullptr;

		const auto& MainFrameModule = IMainFrameModule::Get();
		const TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

		if (MainWindow.IsValid())
		{
			const TSharedPtr<FGenericWindow> NativeWindow = MainWindow->GetNativeWindow();
			if (NativeWindow.IsValid())
			{
				WindowHandle = NativeWindow->GetOSWindowHandle();
			}
		}

		if (WindowHandle == nullptr)
		{
			return false;
		}

		// Launch the file browser.
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform == nullptr)
		{
			return false;
		}

		return DesktopPlatform->OpenFileDialog(
			WindowHandle,
			DialogTitle,
			DefaultPath,
			DefaultFile,
			FileTypes,
			(bIsMultiple ? EFileDialogFlags::Multiple : EFileDialogFlags::None),
			Filenames
		);
	}
}
