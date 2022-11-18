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
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Slate",
				"SlateCore",
				"Settings",
				"SettingsEditor",
				"MainFrame",
				"DesktopPlatform",
				"ImageWriteQueue",
			}
		);
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);
		
		// #TODO: Allows the use of the Stream Deck plugin on Mac.
		if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
		{
			// #TODO: Unable to use functions under construction to support UE 5.1.
			// PublicDefinitions.Add("WITH_STREAM_DECK");
		}
	}
}
