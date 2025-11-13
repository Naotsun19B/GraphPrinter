// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "GraphPrinterGlobals/Utilities/EditorNotification.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "DesktopPlatformModule.h"

namespace GraphPrinter
{
	FString FGraphPrinterUtils::GetImageFileExtension(const EDesiredImageFormat ImageFormat, const bool bWithDot /* = true */)
	{
		FString Dot;
		if (bWithDot)
		{
			Dot = TEXT(".");
		}
		
#if UE_4_22_OR_LATER
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
#else
		if (UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDesiredImageFormat"), true))
		{
			const FString& Extension = EnumPtr->GetNameStringByIndex(static_cast<int32>(ImageFormat));
			return (Dot + FString::Printf(TEXT("%s"), *Extension.ToLower()));
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
			FEditorNotification::Fail(ValidatePathErrorText);
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
		const bool bIsMultiple /* = false */
	)
	{
		// Gets the OS-level window handle of the editor's mainframe.
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		if (ParentWindowHandle == nullptr)
		{
			return false;
		}

		// Launches the file browser.
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform == nullptr)
		{
			return false;
		}

		return DesktopPlatform->OpenFileDialog(
			ParentWindowHandle,
			DialogTitle,
			DefaultPath,
			DefaultFile,
			FileTypes,
			(bIsMultiple ? EFileDialogFlags::Multiple : EFileDialogFlags::None),
			Filenames
		);
	}

	bool FGraphPrinterUtils::ClearUnnecessaryCharactersFromHead(FString& String, const FString& BeginningOfString)
	{
		int32 StartPosition = 0;
		const int32 TextLength = String.Len();
		const int32 HeaderLength = BeginningOfString.Len();
		for (int32 Index = 0; Index < TextLength - HeaderLength; Index++)
		{
			bool bIsMatch = true;
			for (int32 Offset = 0; Offset < HeaderLength; Offset++)
			{
				if (String[Index + Offset] != BeginningOfString[Offset])
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
			String = String.Mid(StartPosition, TextLength - StartPosition);
			return true;
		}

		return false;
	}
}
