// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GraphPrinterGlobals : ModuleRules
{
	public GraphPrinterGlobals(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);
	}
}