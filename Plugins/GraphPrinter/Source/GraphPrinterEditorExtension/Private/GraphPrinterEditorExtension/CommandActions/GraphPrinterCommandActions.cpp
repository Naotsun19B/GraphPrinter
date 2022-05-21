// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "Misc/Paths.h"

namespace GraphPrinter
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	void FGraphPrinterCommandActions::CopyGraphWithAllNodesToClipboard()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::Clipboard;

			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				WidgetPrinter->PrintWidget(WidgetPrinter->CreateDefaultPrintOptions());
			}
		}
	}

	bool FGraphPrinterCommandActions::CanCopyGraphWithAllNodesToClipboard()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::Clipboard;
			
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
		
		return false;
	}

	void FGraphPrinterCommandActions::CopyGraphWithSelectedNodesToClipboard()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::Selected;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::Clipboard;
			
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				WidgetPrinter->PrintWidget(WidgetPrinter->CreateDefaultPrintOptions());
			}
		}
	}

	bool FGraphPrinterCommandActions::CanCopyGraphWithSelectedNodesToClipboard()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
        {
            Options->PrintScope = UPrintWidgetOptions::EPrintScope::Selected;
            Options->ExportMethod = UPrintWidgetOptions::EExportMethod::Clipboard;
            return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
        }
        
        return false;
	}
#endif

	void FGraphPrinterCommandActions::PrintGraphWithAllNodes()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::ImageFile;
			
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				WidgetPrinter->PrintWidget(WidgetPrinter->CreateDefaultPrintOptions());
			}
		}
	}

	bool FGraphPrinterCommandActions::CanPrintGraphWithAllNodes()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::ImageFile;
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
        
		return false;
	}

	void FGraphPrinterCommandActions::PrintGraphWithSelectedNodes()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::Selected;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::ImageFile;
			
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				WidgetPrinter->PrintWidget(WidgetPrinter->CreateDefaultPrintOptions());
			}
		}
	}

	bool FGraphPrinterCommandActions::CanPrintGraphWithSelectedNodes()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::Selected;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::ImageFile;
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
        
		return false;
	}

#ifdef WITH_TEXT_CHUNK_HELPER
	void FGraphPrinterCommandActions::RestoreNodesFromPngFile()
	{
		if (auto* Options = CreateDefaultRestoreOptions<UWidgetPrinter>())
		{
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				WidgetPrinter->RestoreWidget(WidgetPrinter->CreateDefaultRestoreOptions());
			}
		}
	}

	bool FGraphPrinterCommandActions::CanExecuteRestoreWidget()
	{
		if (auto* Options = CreateDefaultRestoreOptions<UWidgetPrinter>())
		{
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}

		return false;
	}
#endif

	void FGraphPrinterCommandActions::OpenExportDestinationFolder()
	{
		//FGraphPrinterUtils::OpenFolderWithExplorer(
		//	UGraphPrinterSettings::Get().OutputDirectory.Path
		//);
	}

	bool FGraphPrinterCommandActions::CanOpenExportDestinationFolder()
	{
		//return FPaths::DirectoryExists(UGraphPrinterSettings::Get().OutputDirectory.Path);
		return false;
	}

	void FGraphPrinterCommandActions::OpenPluginSettings()
	{
		//UGraphPrinterSettings::OpenSettings();
	}
}
