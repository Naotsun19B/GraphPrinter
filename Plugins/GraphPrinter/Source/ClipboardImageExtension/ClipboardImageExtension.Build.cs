// Copyright 2020-2023 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ClipboardImageExtension : ModuleRules
{
	public ClipboardImageExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		
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
				"ImageWrapper",
				"ImageWriteQueue",
				"ApplicationCore",
				
				"GraphPrinterGlobals",
			}
		);

		// #TODO: Allows images to be copied to the clipboard on Mac.
		if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows) ||
		    Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
		{
			PublicDefinitions.Add("WITH_CLIPBOARD_IMAGE_EXTENSION");
		}
	}
}
