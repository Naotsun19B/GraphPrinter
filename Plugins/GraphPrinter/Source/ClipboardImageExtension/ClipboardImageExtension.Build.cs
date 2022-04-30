// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ClipboardImageExtension : ModuleRules
{
	public ClipboardImageExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] 
			{
				Path.Combine(ModuleDirectory, "Public", "ClipboardImageExtension"),
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
				"ImageWrapper",
				"ImageWriteQueue",
				"ApplicationCore",
				
				"GraphPrinterGlobals",
			}
		);

		if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
		{
			PublicDefinitions.Add("WITH_CLIPBOARD_IMAGE_EXTENSION");
		}
	}
}
