// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GraphPrinterCore : ModuleRules
{
	public GraphPrinterCore(ReadOnlyTargetRules Target) : base(Target)
	{
		OptimizeCode = CodeOptimization.Never;
		
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
				"InputCore",
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"MainFrame",
				"ImageWriteQueue",
				"DesktopPlatform",

				"GraphPrinterGlobals",
                "TextChunkHelper",
                "ClipboardImageExtension",
            }
		);

        // To use SGraphEditorImpl.
        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
            }
        );
	}
}
