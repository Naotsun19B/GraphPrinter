// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter/Utilities/WidgetPrinterUtils.h"
#include "WidgetPrinter/Utilities/GraphPrinterSettings.h"
#include "Misc/Paths.h"
#include "ImageWriteTypes.h"

URestoreWidgetOptions::URestoreWidgetOptions()
	: DialogTitle(TEXT("Select the png file that contains the widget info"))
	, DefaultPath(UGraphPrinterSettings::Get().OutputDirectory.Path)
	, DefaultFile(TEXT(""))
	, TargetWidget(nullptr)
{
	SetFileTypesFromImageFormat(EDesiredImageFormat::PNG);
}

FString URestoreWidgetOptions::GetSourceImageFilePath() const
{
	check(HasValidSourceImageFilePath());
	return SourceImageFilePath.GetValue();
}

void URestoreWidgetOptions::SetSourceImageFilePath(const FString& InSourceImageFilePath)
{
	SourceImageFilePath = InSourceImageFilePath;
}

bool URestoreWidgetOptions::HasValidSourceImageFilePath() const
{
	if (SourceImageFilePath.IsSet())
	{
		return FPaths::FileExists(SourceImageFilePath.GetValue());
	}

	return false;
}

void URestoreWidgetOptions::SetFileTypesFromImageFormat(const EDesiredImageFormat ImageFormat)
{
	const FString FormatString = GraphPrinter::FWidgetPrinterUtils::GetImageFileExtension(ImageFormat);
	FileTypes = FString::Printf(
		TEXT("%s Image (.%s)|*.%s"),
		*FormatString.ToUpper(),
		*FormatString.ToLower(),
		*FormatString.ToLower()
	);
}
