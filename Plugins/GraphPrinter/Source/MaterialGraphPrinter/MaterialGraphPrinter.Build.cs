// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MaterialGraphPrinter : ModuleRules
{
	public MaterialGraphPrinter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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
				"Engine",
				"UnrealEd",
				"Slate",
				"SlateCore",

				"GraphPrinterGlobals",
				"GraphPrinter",
				"PreviewViewportPrinter",
			}
		);
		
		
	}
}
