// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "RestoreWidgetOptions.generated.h"

class SWidget;
enum class EDesiredImageFormat : uint8;

/**
 * An optional class to specify when restore the state of the widget.
 */
UCLASS()
class WIDGETPRINTER_API URestoreWidgetOptions : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor.
	URestoreWidgetOptions();

	// Creates and returns a duplicate as the specified option class.
	virtual URestoreWidgetOptions* Duplicate(const TSubclassOf<URestoreWidgetOptions>& DestinationClass) const;
	template<class TRestoreOptions>
	TRestoreOptions* Duplicate()
	{
		static_assert(TIsDerivedFrom<TRestoreOptions, URestoreWidgetOptions>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of URestoreWidgetOptions.");

		return Cast<TRestoreOptions>(Duplicate(TRestoreOptions::StaticClass()));
	}

	// Returns the path of the image file that will be the source to restore.
	// Do not call when no valid value is set.
	FString GetSourceImageFilePath() const;

	// Sets the path of the image file that will be the source to restore.
	void SetSourceImageFilePath(const FString& InSourceImageFilePath);
	
	// Returns whether a valid image file path is specified.
	bool HasValidSourceImageFilePath() const;

	// Generates a File Types string from the image file types.
	void SetFileTypesFromImageFormat(const EDesiredImageFormat ImageFormat);
	
public:
	// The title that appears in the dialog window that lets the user specify the file to use for the restore.
	FString DialogTitle;

	// The path to the directory that is first opened in a dialog window that allows the user to specify the files to use for restoration.
	FString DefaultPath;

	// The path to the file that is first opened in a dialog window that allows the user to specify the files to use for restoration.
	FString DefaultFile;

	// The file type that you can select in the dialog window where you can specify the file that the user will use for the restore.
	FString FileTypes;
	
	// Used when you want to specify the widget to print as an argument.
	TSharedPtr<SWidget> SearchTarget;

private:
	// The path of the image file that will be the source to restore.
	TOptional<FString> SourceImageFilePath;
};
