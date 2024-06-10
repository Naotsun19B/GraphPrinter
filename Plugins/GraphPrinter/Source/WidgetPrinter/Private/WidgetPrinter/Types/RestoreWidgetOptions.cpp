// Copyright 2020-2024 Naotsun. All Rights Reserved.

#include "WidgetPrinter/Types/RestoreWidgetOptions.h"
#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "ImageWriteTypes.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(RestoreWidgetOptions)
#endif

URestoreWidgetOptions::URestoreWidgetOptions()
	: DialogTitle(TEXT("Select the png file that contains the widget info"))
	, DefaultPath(UWidgetPrinterSettings::Get().OutputDirectory.Path)
	, DefaultFile(TEXT(""))
	, SearchTarget(nullptr)
{
	SetFileTypesFromImageFormat(EDesiredImageFormat::PNG);
}

URestoreWidgetOptions* URestoreWidgetOptions::Duplicate(const TSubclassOf<URestoreWidgetOptions>& DestinationClass) const
{
	auto* Destination = NewObject<URestoreWidgetOptions>(GetTransientPackage(), DestinationClass);
	if (IsValid(Destination))
	{
		Destination->DialogTitle = DialogTitle;
		Destination->DefaultPath = DefaultPath;
		Destination->DefaultFile = DefaultFile;
		Destination->FileTypes = FileTypes;
		Destination->SearchTarget = SearchTarget;
		Destination->SourceImageFilePath = SourceImageFilePath;
	}

	return Destination;
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
	const FString FormatString = GraphPrinter::FGraphPrinterUtils::GetImageFileExtension(ImageFormat, false);
	FileTypes = FString::Printf(
		TEXT("%s Image (.%s)|*.%s"),
		*FormatString.ToUpper(),
		*FormatString.ToLower(),
		*FormatString.ToLower()
	);
}
