// Copyright 2020-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class TextChunkHelper : ModuleRules
{
	public TextChunkHelper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
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
