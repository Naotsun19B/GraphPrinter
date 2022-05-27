// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ReferenceViewerPrinter : ModuleRules
{
	public ReferenceViewerPrinter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(
			new string[] 
			{
				Path.Combine(ModuleDirectory, "Private"),
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"UMG",
				"Slate",
				"SlateCore",
				"Engine",
				"UnrealEd",
				"MainFrame",
				"RenderCore",
				"AssetManagerEditor",

				"GraphPrinterGlobals",
				"WidgetPrinter",
				"GenericGraphPrinter",
				"TextChunkHelper",
				"ClipboardImageExtension",
			}
		);
	}
}
