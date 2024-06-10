// Copyright 2020-2024 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ClipboardImageExtension : ModuleRules
{
	public ClipboardImageExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
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

		PublicDefinitions.Add("WITH_CLIPBOARD_IMAGE_EXTENSION");
	}
}
