// Copyright 2020 Naotsun. All Rights Reserved.

#include "GraphPrinterCore.h"
#include "GraphPrinterGlobals.h"
#include "GraphPrinterSettings.h"
#include "PngTextChunkHelpers.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Slate/WidgetRenderer.h"
#include "Widgets/SWidget.h"
#include "GraphEditor.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphUtilities.h"
#include "HAL/FileManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"

#if !BEFORE_UE_4_21
#include "ReferenceViewer/EdGraph_ReferenceViewer.h"
#endif

#define LOCTEXT_NAMESPACE "GraphPrinter"

namespace GraphPrinterCoreDefine
{
	// Number of attempts to draw the widget on the render target.
	// The drawing result may be corrupted once.
	// Probably if draw twice, the drawing result will not be corrupted.
	static const int32 DrawTimes = 2;

	// Key used when writing to a text chunk of a png file.
	static const FString PngTextChunkKey = TEXT("GraphEditor");

	// The beginning of the node information.
	static const FString NodeInfoHeader = TEXT("Begin Object");
}

const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Pending = SNotificationItem::ECompletionState::CS_Pending;
const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Success = SNotificationItem::ECompletionState::CS_Success;
const FGraphPrinterCore::TCompletionState FGraphPrinterCore::CS_Fail = SNotificationItem::ECompletionState::CS_Fail;

TSharedPtr<SNotificationItem> FGraphPrinterCore::ShowNotification(
	const FText& NotificationText, 
	TCompletionState CompletionState, 
	float ExpireDuration /* = 4.f */,
	ENotificationInteraction InteractionType /* = ENotificationInteraction::None */,
	const FText& InteractionText /* = FText()*/,
	FSimpleDelegate InteractionCallback /* = nullptr*/
)
{
	FNotificationInfo NotificationInfo(NotificationText);
	NotificationInfo.bFireAndForget = (ExpireDuration > 0.f);
	if (NotificationInfo.bFireAndForget)
	{
		NotificationInfo.ExpireDuration = ExpireDuration;
	}
	NotificationInfo.bUseLargeFont = true;

	auto StateEnum = static_cast<SNotificationItem::ECompletionState>(CompletionState);
	switch (InteractionType)
	{
	case ENotificationInteraction::Hyperlink:
	{
		NotificationInfo.HyperlinkText = InteractionText;
		NotificationInfo.Hyperlink = InteractionCallback;
		break;
	}
	case ENotificationInteraction::Button:
	{
		FNotificationButtonInfo ButtonInfo(InteractionText, FText(), InteractionCallback, StateEnum);
		NotificationInfo.ButtonDetails.Add(ButtonInfo);
		break;
	}
	default: break;
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
	if (SearchTarget.IsValid())
	{
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
}

TSharedPtr<SGraphEditor> FGraphPrinterCore::FindGraphEditor(TSharedPtr<SWidget> SearchTarget)
{
	TArray<TSharedPtr<SWidget>> ChildWidgets;
	CollectAllChildWidgets(SearchTarget, ChildWidgets);

	for (const auto& ChildWidget : ChildWidgets)
	{
		TSharedPtr<SGraphEditor> GraphEditor = CAST_SLATE_WIDGET(SGraphEditor, ChildWidget);
		if (GraphEditor.IsValid())
		{
			return GraphEditor;
		}
	}

	return nullptr;
}

UTextureRenderTarget2D* FGraphPrinterCore::DrawWidgetToRenderTarget(
	TSharedPtr<SWidget> WidgetToRender,
	const FVector2D& DrawSize,
	bool bUseGamma,
	TextureFilter Filter
)
{
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
	for (int32 Count = 0; Count < GraphPrinterCoreDefine::DrawTimes; Count++)
	{
		WidgetRenderer->DrawWidget(RenderTarget, WidgetToRender.ToSharedRef(), DrawSize, 0.f, false);
		FlushRenderingCommands();
	}

	BeginCleanup(WidgetRenderer);

	return RenderTarget;
}

FString FGraphPrinterCore::CreateFilename(TSharedPtr<SGraphEditor> GraphEditor, const FPrintGraphOptions& Options)
{
	FString Filename = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(Options.OutputDirectoryPath, FGraphPrinterCore::GetGraphTitle(GraphEditor))
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
	if (!GraphEditor.IsValid())
	{
		return false;
	}

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
	if (GraphEditor.IsValid())
	{
		if (UEdGraph* Graph = GraphEditor->GetCurrentGraph())
		{
			// The reference viewer replaces the name because there is no outer object.
#if !BEFORE_UE_4_21
			if (auto* ReferenceViewer = Cast<UEdGraph_ReferenceViewer>(Graph))
			{
				return TEXT("ReferenceViewer");
			}
#endif
			if (UObject* Outer = Graph->GetOuter())
			{
				return FString::Printf(TEXT("%s-%s"), *Outer->GetName(), *Graph->GetName());
			}
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
	if (UEnum* EnumPtr = StaticEnum<EDesiredImageFormat>())
	{
		const FString& EnumString = EnumPtr->GetValueAsString(ImageFormat);
		FString UnuseString;
		FString Extension;
		if (EnumString.Split(TEXT("::"), &UnuseString, &Extension))
		{
			Extension = Extension.ToLower();
			return FString::Printf(TEXT(".%s"), *Extension);
		}
	}
#endif
	
	checkNoEntry();
	return FString();
}

void FGraphPrinterCore::OpenFolderWithExplorer(const FString& FilePath)
{
	const FString& FullFilePath = FPaths::ConvertRelativePathToFull(FilePath);

	FText ValidatePathErrorText;
	if (!FPaths::ValidatePath(FullFilePath, &ValidatePathErrorText))
	{
		ShowNotification(ValidatePathErrorText, CS_Fail);
		return;
	}

	FPlatformProcess::ExploreFolder(*FullFilePath);
}

#ifdef ENABLE_EMBED_NODE_INFO
bool FGraphPrinterCore::ExportGraphToPngFile(const FString& FilePath, TSharedPtr<SGraphEditor> GraphEditor, const FGraphPanelSelectionSet& NodesToExport)
{
	if (!GraphEditor.IsValid())
	{
		return false;
	}

	FPngTextChunkWriter Writer(FilePath);

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(NodesToExport, ExportedText);

	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ExportedText))
	{
		return false;
	}

	return Writer.WriteTextChunk(GraphPrinterCoreDefine::PngTextChunkKey, ExportedText);
}

bool FGraphPrinterCore::RestoreGraphFromPngFile(const FString& FilePath, TSharedPtr<SGraphEditor> GraphEditor)
{
	if (!GraphEditor.IsValid())
	{
		return false;
	}

	FPngTextChunkReader Reader(FilePath);

	TMap<FString, FString> TextChunk;
	if (!Reader.ReadTextChunk(TextChunk))
	{
		return false;
	}

	// Find information on valid nodes.
	if (!TextChunk.Contains(GraphPrinterCoreDefine::PngTextChunkKey))
	{
		return false;
	}
	FString TextToImport = TextChunk[GraphPrinterCoreDefine::PngTextChunkKey];

	// Unnecessary characters may be mixed in at the beginning of the text, so inspect and correct it.
	const int32 TextLength = TextToImport.Len();
	const int32 HeaderLength = GraphPrinterCoreDefine::NodeInfoHeader.Len();
	int32 Index;
	for (Index = 0; Index < (TextLength - HeaderLength); Index++)
	{
		if (TextToImport.Mid(Index, Index + HeaderLength - 1) == GraphPrinterCoreDefine::NodeInfoHeader)
		{
			break;
		}
	}
	if (Index > 0)
	{
		TextToImport = TextToImport.Mid(Index, TextLength - Index);
	}

	if (!FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport))
	{
		return false;
	}

	TSet<UEdGraphNode*> ImportedNodeSet;
	FEdGraphUtilities::ImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport, ImportedNodeSet);
	
	return true;
}

bool FGraphPrinterCore::OpenFileDialog(
	TArray<FString>& FilePaths, 
	const FString& DialogTitle /* = TEXT("Open File Dialog") */,
	const FString& DefaultPath /* = TEXT("") */,
	const FString& DefaultFile /* = TEXT("") */,
	const FString& FileTypes /* = TEXT("All (*)|*.*") */, 
	bool bIsMultiple /* = false */
)
{
	// Get the OS-level window handle of the editor's mainframe.
	void* WindowHandle = nullptr;

	IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
	TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

	if (MainWindow.IsValid())
	{
		TSharedPtr<FGenericWindow> NativeWindow = MainWindow->GetNativeWindow();
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
		FilePaths
	);
}

bool FGraphPrinterCore::GetKeyEventFromUICommandInfo(const TSharedPtr<FUICommandInfo>& UICommandInfo, FKeyEvent& OutKeyEvent)
{
	if (!UICommandInfo.IsValid())
	{
		return false;
	}
	const TSharedRef<const FInputChord>& Chord = UICommandInfo->GetFirstValidChord();

	FModifierKeysState ModifierKeys(
		Chord->bShift, Chord->bShift,
		Chord->bCtrl, Chord->bCtrl,
		Chord->bAlt, Chord->bAlt,
		Chord->bCmd, Chord->bCmd,
		false
	);
	const uint32* CharacterCodePtr;
	const uint32* KeyCodePtr;
	FInputKeyManager::Get().GetCodesFromKey(Chord->Key, CharacterCodePtr, KeyCodePtr);
	uint32 CharacterCode = (CharacterCodePtr != nullptr ? *CharacterCodePtr : 0);
	uint32 KeyCode = (KeyCodePtr != nullptr ? *KeyCodePtr : 0);
	FKeyEvent KeyEvent(Chord->Key, ModifierKeys, FSlateApplication::Get().GetUserIndexForKeyboard(), false, CharacterCode, KeyCode);
	OutKeyEvent = KeyEvent;

	return true;
}
#endif

#undef LOCTEXT_NAMESPACE
