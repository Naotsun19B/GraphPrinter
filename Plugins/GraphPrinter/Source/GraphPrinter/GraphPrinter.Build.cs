// Copyright 2020-2021 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GraphPrinter : ModuleRules
{
	// With this flag can turn on / off the function to embed node information in the png image.
	private readonly bool bEnableEmbedNodeInfo = true;

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
                "ApplicationCore",
                "DesktopPlatform",
                "ImageWriteQueue",
                "AssetManagerEditor",
            }
			);

        // To use SGraphEditorImpl.
        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
            }
            );

        if (bEnableEmbedNodeInfo)
		{
            AddEngineThirdPartyPrivateStaticDependencies(Target,
				"zlib",
				"UElibPNG"
            );

			PublicDefinitions.Add("WITH_EMBED_NODE_INFO");
        }
    }
}
