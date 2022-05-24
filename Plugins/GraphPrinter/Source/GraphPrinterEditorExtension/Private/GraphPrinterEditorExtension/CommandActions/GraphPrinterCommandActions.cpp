// Copyright 2020-2022 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterSettings.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "Misc/Paths.h"

namespace GraphPrinter
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	void FGraphPrinterCommandActions::CopyAllAreaOfWidgetToClipboard()
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

	bool FGraphPrinterCommandActions::CanCopyAllAreaOfWidgetToClipboard()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::Clipboard;
			
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
		
		return false;
	}

	void FGraphPrinterCommandActions::CopySelectedAreaOfWidgetToClipboard()
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

	bool FGraphPrinterCommandActions::CanCopySelectedAreaOfWidgetToClipboard()
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

	void FGraphPrinterCommandActions::PrintAllAreaOfWidget()
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

	bool FGraphPrinterCommandActions::CanPrintAllAreaOfWidget()
	{
		if (auto* Options = CreateDefaultPrintOptions<UWidgetPrinter>())
		{
			Options->PrintScope = UPrintWidgetOptions::EPrintScope::All;
			Options->ExportMethod = UPrintWidgetOptions::EExportMethod::ImageFile;
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
        
		return false;
	}

	void FGraphPrinterCommandActions::PrintSelectedAreaOfWidget()
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

	bool FGraphPrinterCommandActions::CanPrintSelectedAreaOfWidget()
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
	void FGraphPrinterCommandActions::RestoreWidgetFromImageFile()
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
		FGraphPrinterUtils::OpenFolderWithExplorer(
			UWidgetPrinterSettings::Get().OutputDirectory.Path
		);
	}

	bool FGraphPrinterCommandActions::CanOpenExportDestinationFolder()
	{
		return FPaths::DirectoryExists(UWidgetPrinterSettings::Get().OutputDirectory.Path);
	}
}
