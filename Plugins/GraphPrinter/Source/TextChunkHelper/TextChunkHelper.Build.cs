// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class TextChunkHelper : ModuleRules
{
	public TextChunkHelper(ReadOnlyTargetRules Target) : base(Target)
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
				"ImageWrapper",
				"ImageWriteQueue",
				
				"GraphPrinterGlobals"
            }
		);

		AddEngineThirdPartyPrivateStaticDependencies(
			Target,
			"zlib",
			"UElibPNG"
		);
		
		PublicDefinitions.Add("WITH_TEXT_CHUNK_HELPER");
    }
}
