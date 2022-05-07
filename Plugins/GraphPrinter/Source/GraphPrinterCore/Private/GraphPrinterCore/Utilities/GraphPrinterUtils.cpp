// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/Utilities/GraphPrinterUtils.h"
#include "GraphPrinterCore/Utilities/CastSlateWidget.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IMainFrameModule.h"
#include "DesktopPlatformModule.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/SDockingTabStack.h"
#include "SGraphEditorImpl.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

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

	namespace GraphPrinterUtilsConstant
	{
		// The name of the minimap widget class.
		static const FName GraphMinimapClassName = TEXT("SGraphMinimap");
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

	bool FGraphPrinterUtils::GetKeyEventFromUICommandInfo(
		const TSharedPtr<FUICommandInfo>& UICommandInfo,
		FKeyEvent& OutKeyEvent
	)
	{
		check(UICommandInfo.IsValid());
	
		const TSharedRef<const FInputChord>& Chord = UICommandInfo->GetFirstValidChord();

		const FModifierKeysState ModifierKeys(
			Chord->bShift, Chord->bShift,
			Chord->bCtrl, Chord->bCtrl,
			Chord->bAlt, Chord->bAlt,
			Chord->bCmd, Chord->bCmd,
			false
		);
		const uint32* CharacterCodePtr;
		const uint32* KeyCodePtr;
		FInputKeyManager::Get().GetCodesFromKey(Chord->Key, CharacterCodePtr, KeyCodePtr);
		const uint32 CharacterCode = (CharacterCodePtr != nullptr ? *CharacterCodePtr : 0);
		const uint32 KeyCode = (KeyCodePtr != nullptr ? *KeyCodePtr : 0);
		const FKeyEvent KeyEvent(Chord->Key, ModifierKeys, FSlateApplication::Get().GetUserIndexForKeyboard(), false, CharacterCode, KeyCode);
		OutKeyEvent = KeyEvent;

		return true;
	}

	FString FGraphPrinterUtils::GetImageFileExtension(const EDesiredImageFormat ImageFormat)
	{
#if BEFORE_UE_4_21
		if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDesiredImageFormat"), true))
		{
			const FString& Extension = EnumPtr->GetNameStringByIndex(static_cast<int32>(ImageFormat));
			return FString::Printf(TEXT(".%s"), *Extension.ToLower());
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
				return FString::Printf(TEXT(".%s"), *Extension);
			}
		}
#endif
	
		checkNoEntry();
		return {};
	}

	void FGraphPrinterUtils::EnumerateChildWidgets(
		TSharedPtr<SWidget> SearchTarget,
		TFunction<bool(const TSharedPtr<SWidget> ChildWidget)> Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return;
		}

		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			if (ChildWidget.IsValid())
			{
				if (Predicate(ChildWidget))
				{
					EnumerateChildWidgets(ChildWidget, Predicate);
				}
			}
		}
	}

	void FGraphPrinterUtils::EnumerateParentWidgets(
		TSharedPtr<SWidget> SearchTarget,
		TFunction<bool(const TSharedPtr<SWidget> ParentWidget)> Predicate
	)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}
			
		const TSharedPtr<SWidget> ParentWidget = SearchTarget->GetParentWidget();
		if (!ParentWidget.IsValid())
		{
			return;
		}

		if (Predicate(ParentWidget))
		{
			EnumerateParentWidgets(ParentWidget, Predicate);
		}
	}

	TSharedPtr<SDockingTabStack> FGraphPrinterUtils::FindNearestParentDockingTabStack(TSharedPtr<SDockTab> SearchTarget)
	{
		TSharedPtr<SDockingTabStack> FoundDockingTabStack = nullptr;
		
		EnumerateParentWidgets(
			SearchTarget,
			[&FoundDockingTabStack](const TSharedPtr<SWidget> ParentWidget) -> bool
			{
				const TSharedPtr<SDockingTabStack> DockingTabStack = GP_CAST_SLATE_WIDGET(SDockingTabStack, ParentWidget);
				if (DockingTabStack.IsValid())
				{
					FoundDockingTabStack = DockingTabStack;
					return false;
				}

				return true;
			}
		);

		return FoundDockingTabStack;
	}

	TSharedPtr<SGraphEditorImpl> FGraphPrinterUtils::FindNearestChildGraphEditor(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SGraphEditorImpl> FoundGraphEditor = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundGraphEditor](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SGraphEditorImpl> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, ChildWidget);
				if (GraphEditor.IsValid())
				{
					FoundGraphEditor = GraphEditor;
					return false;
				}

				return true;
			}
		);

		return FoundGraphEditor;
	}

	TSharedPtr<SGraphEditorImpl> FGraphPrinterUtils::GetActiveGraphEditor()
	{
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		const TSharedPtr<SDockTab> ActiveTab = GlobalTabManager->GetActiveTab();
		if (!ActiveTab.IsValid())
		{
			return nullptr;
		}
	
		const TSharedPtr<SDockingTabStack> DockingTabStack = FindNearestParentDockingTabStack(ActiveTab);
		if (!DockingTabStack.IsValid())
		{
			return nullptr;
		}

		return FindNearestChildGraphEditor(DockingTabStack);
	}

	TSharedPtr<SWidget> FGraphPrinterUtils::FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SWidget> FoundMinimap = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundMinimap](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SWidget> Minimap = Private::CastSlateWidget<SWidget>(
					ChildWidget,
					GraphPrinterUtilsConstant::GraphMinimapClassName
				);
				if (Minimap.IsValid())
				{
					FoundMinimap = Minimap;
					return false;
				}

				return true;
			}
		);

		return FoundMinimap;
	}

	TSharedPtr<SOverlay> FGraphPrinterUtils::FindNearestChildOverlay(TSharedPtr<SWidget> SearchTarget)
	{
		TSharedPtr<SOverlay> FoundOverlay = nullptr;
		
		EnumerateChildWidgets(
			SearchTarget,
			[&FoundOverlay](const TSharedPtr<SWidget> ChildWidget) -> bool
			{
				const TSharedPtr<SOverlay> Overlay = GP_CAST_SLATE_WIDGET(SOverlay, ChildWidget);
				if (Overlay.IsValid())
				{
					FoundOverlay = Overlay;
					return false;
				}

				return true;
			}
		);

		return FoundOverlay;
	}

	TArray<TSharedPtr<STextBlock>> FGraphPrinterUtils::GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget)
	{
		if (!SearchTarget.IsValid())
		{
			return {};
		}

		FChildren* Children = SearchTarget->GetChildren();
		if (Children == nullptr)
		{
			return {};
		}

		TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks;
		for (int32 Index = 0; Index < Children->Num(); Index++)
		{
			const TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
			TSharedPtr<STextBlock> TextBlock = GP_CAST_SLATE_WIDGET(STextBlock, ChildWidget);
			if (TextBlock.IsValid())
			{
				if (TextBlock->GetVisibility().IsVisible())
				{
					VisibleChildTextBlocks.Add(TextBlock);
				}
			}
		}
		
		return VisibleChildTextBlocks;
	}
}
