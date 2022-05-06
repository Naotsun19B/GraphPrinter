// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterCore/Utilities/GraphPrinterSettings.h"
#include "GraphPrinterCore/WidgetPrinter/WidgetPrinter.h"
#include "GraphPrinterCore/WidgetPrinter/GenericGraphPrinter.h"
#include "GraphPrinterGlobals/GraphPrinterGlobals.h"
#include "ISettingsModule.h"
#include "ImageWriteTypes.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#define LOCTEXT_NAMESPACE "GraphPrinterSettings"

namespace GraphPrinter
{
	namespace Settings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");
		static const FName SectionName		= TEXT("GraphPrinterSettings");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		}
	}
}

UGraphPrinterSettings::UGraphPrinterSettings()
	: Format(EDesiredImageFormat::PNG)
	, CompressionQuality(0)
	, FilteringMode(TF_Default)
	, bDrawOnlyGraph(false)
	, bUseGamma(true)
	, Padding(0.f)
	, MaxImageSize(15000.f, 15000.f)
	, bCanOverwriteFileWhenExport(false)
	, bHideToolbarComboButton(false)
{
#ifdef WITH_TEXT_CHUNK_HELPER
	bIsIncludeNodeInfoInImageFile = true;
	bWithTextChunkHelper = true;
#else
	bIsIncludeNodeInfoInImageFile = false;
	bWithTextChunkHelper = false;
#endif
	
	OutputDirectory.Path = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(
			FPaths::ProjectSavedDir(),
			GraphPrinter::PluginName.ToString()
		)
	);

	WidgetPrinterClasses.Add(UGenericGraphPrinter::StaticClass());
}

void UGraphPrinterSettings::Register()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->RegisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName,
			LOCTEXT("SettingName", "Graph Printer"),
			LOCTEXT("SettingDescription", "Editor settings for Graph Printer"),
			GetMutableDefault<UGraphPrinterSettings>()
		);
	}
}

void UGraphPrinterSettings::Unregister()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->UnregisterSettings(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName
		);
	}
}

const UGraphPrinterSettings& UGraphPrinterSettings::Get()
{
	const auto* Settings = GetDefault<UGraphPrinterSettings>();
	check(IsValid(Settings));
	return *Settings;
}

void UGraphPrinterSettings::OpenSettings()
{
	if (ISettingsModule* SettingsModule = GraphPrinter::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			GraphPrinter::Settings::ContainerName,
			GraphPrinter::Settings::CategoryName,
			GraphPrinter::Settings::SectionName
		);
	}
}

GraphPrinter::FPrintWidgetOptions UGraphPrinterSettings::GeneratePrintGraphOptions() const
{
	GraphPrinter::FPrintWidgetOptions Options;
#ifdef WITH_TEXT_CHUNK_HELPER
	Options.bIsIncludeNodeInfoInImageFile = bIsIncludeNodeInfoInImageFile;
#endif
	Options.ImageWriteOptions.Format = Format;
	Options.ImageWriteOptions.CompressionQuality = CompressionQuality;
	Options.FilteringMode = FilteringMode;
	Options.bDrawOnlyGraph = bDrawOnlyGraph;
	Options.bUseGamma = bUseGamma;
	Options.Padding = Padding;
	Options.MaxImageSize = MaxImageSize;
	Options.ImageWriteOptions.bOverwriteFile = bCanOverwriteFileWhenExport;
	Options.OutputDirectoryPath = OutputDirectory.Path;
	
	return Options;
}

TArray<UWidgetPrinter*> UGraphPrinterSettings::GetWidgetPrinters() const
{
	TArray<UWidgetPrinter*> Instances;
	Instances.Reserve(WidgetPrinterClasses.Num());

	for (const auto& WidgetPrinterClass : WidgetPrinterClasses)
	{
		if (!IsValid(WidgetPrinterClass))
		{
			continue;
		}

		Instances.Add(Cast<UWidgetPrinter>(WidgetPrinterClass->GetDefaultObject()));
	}
	
	return Instances;
}

void UGraphPrinterSettings::PostInitProperties()
{
	Super::PostInitProperties();

#ifndef WITH_TEXT_CHUNK_HELPER
	bIsIncludeNodeInfoInImageFile = false;
#endif

	auto& FileManager = IFileManager::Get();
	if (!FileManager.DirectoryExists(*OutputDirectory.Path))
	{
		FileManager.MakeDirectory(*OutputDirectory.Path, true);
	}

	ModifyFormat();
	ModifyCompressionQuality();
	ModifyMaxImageSize();
	ModifyWidgetPrinterClasses();
}

void UGraphPrinterSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, bIsIncludeNodeInfoInImageFile))
	{
		ModifyFormat();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, Format) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, CompressionQuality))
	{
		ModifyCompressionQuality();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, MaxImageSize))
	{
		ModifyMaxImageSize();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UGraphPrinterSettings, WidgetPrinterClasses))
	{
		ModifyWidgetPrinterClasses();
	}
}

void UGraphPrinterSettings::ModifyFormat()
{
#ifdef WITH_TEXT_CHUNK_HELPER
	if (bIsIncludeNodeInfoInImageFile)
	{
		Format = EDesiredImageFormat::PNG;
	}
#endif
}

void UGraphPrinterSettings::ModifyCompressionQuality()
{
	int32 Max = 100;
	if (Format == EDesiredImageFormat::EXR)
	{
		Max = 1;
	}
	CompressionQuality = FMath::Clamp(CompressionQuality, 0, Max);
}

void UGraphPrinterSettings::ModifyMaxImageSize()
{
	if (MaxImageSize.X < 0.f)
	{
		MaxImageSize.X = 0.f;
	}
	if (MaxImageSize.Y < 0.f)
	{
		MaxImageSize.Y = 0.f;
	}
}

void UGraphPrinterSettings::ModifyWidgetPrinterClasses()
{
	TArray<TSubclassOf<UWidgetPrinter>> NoElementDuplicate;
	for (const auto& WidgetPrinterClass : WidgetPrinterClasses)
	{
		if (!NoElementDuplicate.Contains(WidgetPrinterClass))
		{
			NoElementDuplicate.Add(WidgetPrinterClass);
		}
	}
	WidgetPrinterClasses = NoElementDuplicate;
	
	WidgetPrinterClasses.Sort(
		[](const TSubclassOf<UWidgetPrinter>& Lhs, const TSubclassOf<UWidgetPrinter>& Rhs) -> bool
		{
			auto GetPriority = [](const TSubclassOf<UWidgetPrinter>& Class) -> int32
			{
				if (IsValid(Class))
				{
					if (const auto* Instance = Cast<UWidgetPrinter>(Class->GetDefaultObject()))
					{
						return Instance->GetPriority();
					}
				}
				
				return TNumericLimits<int32>::Min();
			};

			return (GetPriority(Lhs) < GetPriority(Rhs));
		}
	);
}

#undef LOCTEXT_NAMESPACE
