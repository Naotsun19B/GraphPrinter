// Copyright 2020-2021 Naotsun. All Rights Reserved.

#include "GraphPrinter/GraphPrinterCore.h"
#include "GraphPrinter/GraphPrinterSettings.h"
#include "GraphPrinter/GraphPrinterTypes.h"
#include "GraphPrinter/PngTextChunkHelper.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Slate/WidgetRenderer.h"
#include "Widgets/SWidget.h"
#include "GraphEditor.h"
#include "SGraphEditorImpl.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SOverlay.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphUtilities.h"
#include "HAL/FileManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Engine/TextureRenderTarget2D.h"

#if !BEFORE_UE_4_21
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinter
{
	namespace GraphPrinterCoreInternal
	{
		// Number of attempts to draw the widget on the render target.
		// The drawing result may be corrupted once.
		// Probably if draw twice, the drawing result will not be corrupted.
		static constexpr int32 DrawTimes = 2;

		// Key used when writing to a text chunk of a png file.
		static const FString PngTextChunkKey = TEXT("GraphEditor");

		// The beginning of the node information.
		static const FString NodeInfoHeader = TEXT("Begin Object");

		/**
		 * Cast function for classes that inherit from SWidget.
		 */
		template<class To, class From>
		TSharedPtr<To> CastSlateWidget(TSharedPtr<From> FromPtr, const FName& ToClassName)
		{
			static_assert(TIsDerivedFrom<From, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");
			static_assert(TIsDerivedFrom<To, SWidget>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of SWidget.");

			if (FromPtr.IsValid())
			{
				if (FromPtr->GetType() == ToClassName)
				{
					return StaticCastSharedPtr<To>(FromPtr);
				}
			}

			return nullptr;
		}

		// The name of the minimap widget class.
		static const FName GraphMinimapClassName = TEXT("SGraphMinimap");
		
		// Returns a minimap if it was a child of the graph editor.
		// What to do if Grap Minimap is installed.
		static TSharedPtr<SWidget> FindNearestChildMinimap(TSharedPtr<SWidget> SearchTarget)
		{
			TSharedPtr<SWidget> FoundMinimap = nullptr;
		
			FGraphPrinterCore::EnumerateChildWidgets(
				SearchTarget,
				[&FoundMinimap](const TSharedPtr<SWidget> ChildWidget) -> bool
				{
					const TSharedPtr<SWidget> Minimap = CastSlateWidget<SWidget>(ChildWidget, GraphMinimapClassName);
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
	}
#define GP_CAST_SLATE_WIDGET(ToClass, FromPtr) GraphPrinterCoreInternal::CastSlateWidget<ToClass>(FromPtr, #ToClass)

	constexpr FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Pending = SNotificationItem::ECompletionState::CS_Pending;
	constexpr FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Success = SNotificationItem::ECompletionState::CS_Success;
	constexpr FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Fail = SNotificationItem::ECompletionState::CS_Fail;

	TSharedPtr<SNotificationItem> FGraphPrinterCore::ShowNotification(
		const FText& NotificationText, 
		TCompletionState CompletionState, 
		float ExpireDuration /* = 4.f */,
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
	
		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetCompletionState(StateEnum);
		}
	
		return NotificationItem;
	}

	void FGraphPrinterCore::CollectAllChildWidgets(TSharedPtr<SWidget> SearchTarget, TArray<TSharedPtr<SWidget>>& OutChildren)
	{
		if (!SearchTarget.IsValid())
		{
			return;
		}

		if (FChildren* Children = SearchTarget->GetChildren())
		{
			for (int32 Index = 0; Index < Children->Num(); Index++)
			{
				TSharedPtr<SWidget> ChildWidget = Children->GetChildAt(Index);
				OutChildren.Add(ChildWidget);
				CollectAllChildWidgets(ChildWidget, OutChildren);
			}
		}
	}

	TSharedPtr<SGraphEditor> FGraphPrinterCore::FindGraphEditor(TSharedPtr<SWidget> SearchTarget)
	{
		TArray<TSharedPtr<SWidget>> ChildWidgets;
		CollectAllChildWidgets(SearchTarget, ChildWidgets);

		for (const auto& ChildWidget : ChildWidgets)
		{
			TSharedPtr<SGraphEditor> GraphEditor = GP_CAST_SLATE_WIDGET(SGraphEditorImpl, ChildWidget);
			if (GraphEditor.IsValid())
			{
				return GraphEditor;
			}
		}

		return nullptr;
	}

	UTextureRenderTarget2D* FGraphPrinterCore::DrawGraphToRenderTarget(
		TSharedPtr<SGraphEditor> GraphToRender,
		const FVector2D& DrawSize,
		bool bUseGamma,
		TextureFilter Filter,
		bool bDrawOnlyGraph
	)
	{
		check(GraphToRender.IsValid());

		// If there is a minimap, hide it only while drawing.
		const TSharedPtr<SWidget> Minimap = GraphPrinterCoreInternal::FindNearestChildMinimap(GraphToRender);
		TOptional<EVisibility> PreviousMinimapVisibility;
		if (Minimap.IsValid())
		{
			PreviousMinimapVisibility = Minimap->GetVisibility();
			Minimap->SetVisibility(EVisibility::Collapsed);
		}
		
		// If there is a title bar, hide it only while drawing.
		const TSharedPtr<SWidget> TitleBar = GraphToRender->GetTitleBar();
		TOptional<EVisibility> PreviousTitleBarVisibility;
		if (bDrawOnlyGraph && TitleBar.IsValid())
		{
			PreviousTitleBarVisibility = TitleBar->GetVisibility();
			TitleBar->SetVisibility(EVisibility::Collapsed);
		}

		// Hide zoom magnification and graph type text while drawing.
		TMap<TSharedPtr<STextBlock>, EVisibility> PreviousChildTextBlockVisibilities;
		if (bDrawOnlyGraph)
		{
			const TSharedPtr<SOverlay> Overlay = FindNearestChildOverlay(GraphToRender);
			TArray<TSharedPtr<STextBlock>> VisibleChildTextBlocks = GetVisibleChildTextBlocks(Overlay);
			for (const TSharedPtr<STextBlock>& VisibleChildTextBlock : VisibleChildTextBlocks)
			{
				if (VisibleChildTextBlock.IsValid())
				{
					PreviousChildTextBlockVisibilities.Add(VisibleChildTextBlock, VisibleChildTextBlock->GetVisibility());
					VisibleChildTextBlock->SetVisibility(EVisibility::Collapsed);
				}
			}
		}
		
		FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, false);
		if (WidgetRenderer == nullptr)
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Widget Renderer could not be generated."));
			return nullptr;
		}

		UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGamma);
		if (!IsValid(RenderTarget))
		{
			UE_LOG(LogGraphPrinter, Error, TEXT("Failed to generate RenderTarget."));
			return nullptr;
		}
		if (bUseGamma)
		{
			RenderTarget->bForceLinearGamma = true;
			RenderTarget->UpdateResourceImmediate(true);
		}

		// Since the drawing result may be corrupted the first time, draw multiple times.
		for (int32 Count = 0; Count < GraphPrinterCoreInternal::DrawTimes; Count++)
		{
			WidgetRenderer->DrawWidget(RenderTarget, GraphToRender.ToSharedRef(), DrawSize, 0.f, false);
			FlushRenderingCommands();
		}

		BeginCleanup(WidgetRenderer);

		// Restores the visibility of the title bar,
		// zoom magnification text, and graph type text.
		if (Minimap.IsValid() && PreviousMinimapVisibility.IsSet())
		{
			Minimap->SetVisibility(PreviousMinimapVisibility.GetValue());
		}
		
		if (bDrawOnlyGraph && TitleBar.IsValid() && PreviousTitleBarVisibility.IsSet())
		{
			TitleBar->SetVisibility(PreviousTitleBarVisibility.GetValue());
		}

		if (bDrawOnlyGraph)
		{
			for (const auto& PreviousChildTextBlockVisibility : PreviousChildTextBlockVisibilities)
			{
				TSharedPtr<STextBlock> TextBlock = PreviousChildTextBlockVisibility.Key;
				EVisibility PreviousVisibility = PreviousChildTextBlockVisibility.Value;
				if (TextBlock.IsValid())
				{
					TextBlock->SetVisibility(PreviousVisibility);
				}
			}
		}
		
		return RenderTarget;
	}

	
	void FGraphPrinterCore::EnumerateChildWidgets(
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

	TSharedPtr<SOverlay> FGraphPrinterCore::FindNearestChildOverlay(TSharedPtr<SWidget> SearchTarget)
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
	
	TArray<TSharedPtr<STextBlock>> FGraphPrinterCore::GetVisibleChildTextBlocks(TSharedPtr<SWidget> SearchTarget)
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

	FString FGraphPrinterCore::CreateFilename(TSharedPtr<SGraphEditor> GraphEditor, const FPrintGraphOptions& Options)
	{
		FString Filename = FPaths::ConvertRelativePathToFull(
			FPaths::Combine(Options.OutputDirectoryPath, GetGraphTitle(GraphEditor))
		);
		const FString& Extension = GetImageFileExtension(Options.ImageWriteOptions.Format);

		FText ValidatePathErrorText;
		if (!FPaths::ValidatePath(Filename, &ValidatePathErrorText))
		{
			ShowNotification(ValidatePathErrorText, CS_Fail);
			return FString();
		}

		// If the file cannot be overwritten, add a number after the file name.
		if (!Options.ImageWriteOptions.bOverwriteFile)
		{
			if (IFileManager::Get().FileExists(*FString(Filename + Extension)))
			{
				auto CombineFilenameAndIndex = [](const FString& Filename, int32 Index) -> FString
				{
					return FString::Printf(TEXT("%s_%d"), *Filename, Index);
				};

				int32 Index = 0;
				while (Index < TNumericLimits<int32>().Max())
				{
					const FString& FilenameWithExtension = CombineFilenameAndIndex(Filename, Index) + Extension;
					if (!IFileManager::Get().FileExists(*FilenameWithExtension))
					{
						break;
					}
					Index++;
				}
				Filename = CombineFilenameAndIndex(Filename, Index);
			}
		}

		return (Filename + Extension);
	}

	bool FGraphPrinterCore::CalculateGraphDrawSizeAndViewLocation(
		FVector2D& DrawSize, 
		FVector2D& ViewLocation, 
		TSharedPtr<SGraphEditor> GraphEditor, 
		float Padding
	)
	{
		check(GraphEditor.IsValid());

#if BEFORE_UE_4_23
		// Special support is implemented because SGraphEditor::GetNumberOfSelectedNodes before UE4.23 always returns 0.
		const TSet<UObject*>& SelectedNodes = GraphEditor->GetSelectedNodes();
		if (SelectedNodes.Num() == 0)
		{
			return false;
		}
#else
		if (GraphEditor->GetNumberOfSelectedNodes() == 0)
		{
			return false;
		}
#endif

		FSlateRect Bounds;
		if (!GraphEditor->GetBoundsForSelectedNodes(Bounds, Padding + 100.f))
		{
			return false;
		}
		DrawSize = Bounds.GetSize();
		ViewLocation = Bounds.GetTopLeft();

		return true;
	}

	FString FGraphPrinterCore::GetGraphTitle(TSharedPtr<SGraphEditor> GraphEditor)
	{
		check(GraphEditor.IsValid());

		if (UEdGraph* Graph = GraphEditor->GetCurrentGraph())
		{
#if !BEFORE_UE_4_21
			// The reference viewer replaces the name because there is no outer object.
			if (const auto* ReferenceViewer = Cast<UEdGraph_ReferenceViewer>(Graph))
			{
				const TArray<FAssetIdentifier>& Assets = ReferenceViewer->GetCurrentGraphRootIdentifiers();
				if (Assets.IsValidIndex(0))
				{
					return FString::Printf(TEXT("ReferenceViewer_%s"), *FPaths::GetBaseFilename(Assets[0].PackageName.ToString()));
				}
			
				return TEXT("ReferenceViewer");
			}
#endif
			if (const UObject* Outer = Graph->GetOuter())
			{
				return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			}
		}

		return TEXT("Invalid GraphEditor");
	}

	FString FGraphPrinterCore::GetImageFileExtension(EDesiredImageFormat ImageFormat)
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
		return FString();
	}

	void FGraphPrinterCore::OpenFolderWithExplorer(const FString& Filename)
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

#if ENABLE_EMBED_NODE_INFO
	bool FGraphPrinterCore::ExportGraphToPngFile(const FString& Filename, TSharedPtr<SGraphEditor> GraphEditor, const FGraphPanelSelectionSet& NodesToExport)
	{
		check(GraphEditor.IsValid());

		// Make a shortcut key event for copy operation.
		FKeyEvent KeyEvent;
		if (!GetKeyEventFromUICommandInfo(FGenericCommands::Get().Copy, KeyEvent))
		{
			return false;
		}

		// Since the clipboard is used, the current data is temporarily saved.
		FString CurrentClipboard;
		FPlatformApplicationMisc::ClipboardPaste(CurrentClipboard);

		// Get information about the selected node via the clipboard.
		// Using "FSlateApplication::ProcessKeyDownEvent" seems to be a legitimate access, 
		// but using that may not work outside the main window, so call the graph editor keydown event directly.
#if BEFORE_UE_4_23
		const FReply& Reply = GraphEditor->OnKeyDown(GraphEditor->GetCachedGeometry(), KeyEvent);
#else
		const FReply& Reply = GraphEditor->OnKeyDown(GraphEditor->GetTickSpaceGeometry(), KeyEvent);
#endif

		FString ExportedText;
		FPlatformApplicationMisc::ClipboardPaste(ExportedText);

		// Restore the saved clipboard data.
		FPlatformApplicationMisc::ClipboardCopy(*CurrentClipboard);

		if (!Reply.IsEventHandled() || !FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ExportedText))
		{
			return false;
		}

		// Write data to png file using helper class.
		TMap<FString, FString> MapToWrite;
		MapToWrite.Add(GraphPrinterCoreInternal::PngTextChunkKey, ExportedText);

		const TSharedPtr<FPngTextChunkHelper> PngTextChunkHelper = FPngTextChunkHelper::CreatePngTextChunkHelper(Filename);
		if (!PngTextChunkHelper.IsValid())
		{
			return false;
		}
		return PngTextChunkHelper->Write(MapToWrite);
	}

	bool FGraphPrinterCore::RestoreGraphFromPngFile(const FString& Filename, TSharedPtr<SGraphEditor> GraphEditor)
	{
		check(GraphEditor.IsValid());

		// Read data from png file using helper class.
		TMap<FString, FString> MapToRead;
		const TSharedPtr<FPngTextChunkHelper> PngTextChunkHelper = FPngTextChunkHelper::CreatePngTextChunkHelper(Filename);
		if (!PngTextChunkHelper.IsValid())
		{
			return false;
		}
		if (!PngTextChunkHelper->Read(MapToRead))
		{
			return false;
		}

		// Find information on valid nodes.
		if (!MapToRead.Contains(GraphPrinterCoreInternal::PngTextChunkKey))
		{
			return false;
		}
		FString TextToImport = MapToRead[GraphPrinterCoreInternal::PngTextChunkKey];

		// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
		int32 StartPosition = 0;
		const int32 TextLength = TextToImport.Len();
		const int32 HeaderLength = GraphPrinterCoreInternal::NodeInfoHeader.Len();
		for (int32 Index = 0; Index < TextLength - HeaderLength; Index++)
		{
			bool bIsMatch = true;
			for (int32 Offset = 0; Offset < HeaderLength; Offset++)
			{
				if (TextToImport[Index + Offset] != GraphPrinterCoreInternal::NodeInfoHeader[Offset])
				{
					bIsMatch = false;
				}
			}

			if (bIsMatch)
			{
				StartPosition = Index;
				break;
			}
		}
		if (StartPosition > 0)
		{
			TextToImport = TextToImport.Mid(StartPosition, TextLength - StartPosition);
		}

		if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport))
		{
			return false;
		}

		// Make a shortcut key event for copy operation.
		FKeyEvent KeyEvent;
		if (!GetKeyEventFromUICommandInfo(FGenericCommands::Get().Paste, KeyEvent))
		{
			return false;
		}

		// Since the clipboard is used, the current data is temporarily saved.
		FString CurrentClipboard;
		FPlatformApplicationMisc::ClipboardPaste(CurrentClipboard);

		// Import node information from png image via clipboard.
		FPlatformApplicationMisc::ClipboardCopy(*TextToImport);

		// Using "FSlateApplication::ProcessKeyDownEvent" seems to be a legitimate access, 
		// but using that may not work outside the main window, so call the graph editor keydown event directly.
#if BEFORE_UE_4_23
		const FReply& Reply = GraphEditor->OnKeyDown(GraphEditor->GetCachedGeometry(), KeyEvent);
#else
		const FReply& Reply = GraphEditor->OnKeyDown(GraphEditor->GetTickSpaceGeometry(), KeyEvent);
#endif

		// Restore the saved clipboard data.
		FPlatformApplicationMisc::ClipboardCopy(*CurrentClipboard);

		return Reply.IsEventHandled();
	}

	bool FGraphPrinterCore::OpenFileDialog(
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

		const IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
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

	bool FGraphPrinterCore::GetKeyEventFromUICommandInfo(const TSharedPtr<FUICommandInfo>& UICommandInfo, FKeyEvent& OutKeyEvent)
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
#endif
}
	
#undef GP_CAST_SLATE_WIDGET

#undef LOCTEXT_NAMESPACE
