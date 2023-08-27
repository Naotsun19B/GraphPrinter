// Copyright 2020-2023 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GenericGraphPrinter : ModuleRules
{
	public GenericGraphPrinter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		
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
            }
		);
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				// To use SGraphEditorImpl.
				Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
			}
		);
	}
}
