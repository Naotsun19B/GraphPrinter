// Copyright 2020 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinter : ModuleRules
{
	public GraphPrinter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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
				"UMG",
				"Slate",
				"SlateCore",
				"RenderCore",
				"UnrealEd",
				"GraphEditor",
				"MainFrame",
				"EditorStyle",
                "ImageWriteQueue",
            }
			);
    }
}
