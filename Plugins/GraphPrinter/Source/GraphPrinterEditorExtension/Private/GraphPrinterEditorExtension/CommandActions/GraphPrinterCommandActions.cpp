// Copyright 2020-2025 Naotsun. All Rights Reserved.

#include "GraphPrinterEditorExtension/CommandActions/GraphPrinterCommandActions.h"
#include "GraphPrinterGlobals/Utilities/GraphPrinterUtils.h"
#include "WidgetPrinter/IWidgetPrinterRegistry.h"
#include "WidgetPrinter/Utilities/WidgetPrinterSettings.h"
#include "Misc/Paths.h"

namespace GraphPrinter
{
#ifdef WITH_CLIPBOARD_IMAGE_EXTENSION
	void FGraphPrinterCommandActions::CopyAllAreaOfWidgetToClipboard()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::Clipboard))
		{
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				Options = WidgetPrinter->CreateDefaultPrintOptions(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::Clipboard);
				WidgetPrinter->PrintWidget(Options);
			}
		}
	}

	bool FGraphPrinterCommandActions::CanCopyAllAreaOfWidgetToClipboard()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::Clipboard))
		{
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
		
		return false;
	}

	void FGraphPrinterCommandActions::CopySelectedAreaOfWidgetToClipboard()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::Clipboard))
		{
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				Options = WidgetPrinter->CreateDefaultPrintOptions(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::Clipboard);
				WidgetPrinter->PrintWidget(Options);
			}
		}
	}

	bool FGraphPrinterCommandActions::CanCopySelectedAreaOfWidgetToClipboard()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::Clipboard))
        {
            return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
        }
        
        return false;
	}
#endif

	void FGraphPrinterCommandActions::PrintAllAreaOfWidget()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::ImageFile))
		{
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				Options = WidgetPrinter->CreateDefaultPrintOptions(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::ImageFile);
				WidgetPrinter->PrintWidget(Options);
			}
		}
	}

	bool FGraphPrinterCommandActions::CanPrintAllAreaOfWidget()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::All, UPrintWidgetOptions::EExportMethod::ImageFile))
		{
			return IsValid(IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options));
		}
        
		return false;
	}

	void FGraphPrinterCommandActions::PrintSelectedAreaOfWidget()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::ImageFile))
		{
			if (UWidgetPrinter* WidgetPrinter = IWidgetPrinterRegistry::Get().FindAvailableWidgetPrinter(Options))
			{
				Options = WidgetPrinter->CreateDefaultPrintOptions(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::ImageFile);
				WidgetPrinter->PrintWidget(Options);
			}
		}
	}

	bool FGraphPrinterCommandActions::CanPrintSelectedAreaOfWidget()
	{
		if (UPrintWidgetOptions* Options = CreateDefaultPrintOptions<UWidgetPrinter>(UPrintWidgetOptions::EPrintScope::Selected, UPrintWidgetOptions::EExportMethod::ImageFile))
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
				Options = WidgetPrinter->CreateDefaultRestoreOptions();
				WidgetPrinter->RestoreWidget(Options);
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
		const auto& Settings = GraphPrinter::GetSettings<UWidgetPrinterSettings>();
		FGraphPrinterUtils::OpenFolderWithExplorer(
			Settings.OutputDirectory.Path
		);
	}

	bool FGraphPrinterCommandActions::CanOpenExportDestinationFolder()
	{
		const auto& Settings = GraphPrinter::GetSettings<UWidgetPrinterSettings>();
		return FPaths::DirectoryExists(Settings.OutputDirectory.Path);
	}
}
