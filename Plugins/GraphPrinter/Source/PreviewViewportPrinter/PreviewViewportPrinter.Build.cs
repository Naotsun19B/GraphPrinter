// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PreviewViewportPrinter : ModuleRules
{
	public PreviewViewportPrinter(ReadOnlyTargetRules Target) : base(Target)
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

				"GraphPrinterGlobals",
				"WidgetPrinter",
				"TextChunkHelper",
				"ClipboardImageExtension",
				
				// #TODO: Need a graph editor to get the asset name in the preview viewport.
				"GenericGraphPrinter"
			}
		);
	}
}
