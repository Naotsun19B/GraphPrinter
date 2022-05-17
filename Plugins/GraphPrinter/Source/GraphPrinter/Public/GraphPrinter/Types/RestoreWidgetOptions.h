// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"

class SWidget;

#ifdef WITH_TEXT_CHUNK_HELPER
namespace GraphPrinter
{
	/**
	 * An optional structure to specify when restore the state of the widget.
	 */
	struct FRestoreWidgetOptions
	{
	public:
		// Constructor.
		FRestoreWidgetOptions()
			: TargetWidget(nullptr)
		{
		}
		
		explicit FRestoreWidgetOptions(const FString& InSourceImageFilePath)
			: TargetWidget(nullptr)
			, SourceImageFilePath(InSourceImageFilePath)
		{
		}

		// Returns the path of the image file that will be the source to restore.
		FString GetSourceImageFilePath() const
		{
			check(HasValidSourceImageFilePath());
			return SourceImageFilePath.GetValue();
		}

		// Returns whether a valid image file path is specified.
		bool HasValidSourceImageFilePath() const
		{
			if (SourceImageFilePath.IsSet())
			{
				return FPaths::FileExists(SourceImageFilePath.GetValue());
			}

			return false;
		}

	public:
		// Used when you want to specify the widget to print as an argument.
		TSharedPtr<SWidget> TargetWidget;

	private:
		// The path of the image file that will be the source to restore.
		TOptional<FString> SourceImageFilePath;
	};
}
#endif
