// Copyright 2020 Naotsun. All Rights Reserved.

using System.IO;
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
                "DesktopPlatform",
                "ImageWriteQueue",
                "Kismet",
                "MaterialEditor",
                "AIModule",
                "AIGraph",
                "BehaviorTreeEditor",
                "EnvironmentQueryEditor",
                "AudioEditor",
                "ControlRig",
                "ControlRigEditor",
                "ControlRigDeveloper",
                "RigVMDeveloper",
                "AnimGraph",
                "AssetManagerEditor",
            }
            );

        AddEngineThirdPartyPrivateStaticDependencies(Target,
            "zlib",
            "UElibPNG"
        );

        PrivateIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(EditorSourcePath, "MaterialEditor", "Private"),
                Path.Combine(EditorSourcePath, "BehaviorTreeEditor", "Private"),
                Path.Combine(GetPluginSourcePath("AI", "EnvironmentQueryEditor"), "EnvironmentQueryEditor", "Private"),
                Path.Combine(EditorSourcePath, "AudioEditor", "Private"),
            }
            );
    }

    private string EnginePath
    {
        get
        {
            return Path.GetFullPath(EngineDirectory);
        }
    }
    
    private string EditorSourcePath
    {
        get
        {
            return Path.Combine(EnginePath, "Source", "Editor");
        }
    }

    private string GetPluginSourcePath(params string[] PluginPath)
    {
        string CombinedPluginPath = Path.Combine(PluginPath);
        return Path.Combine(EnginePath, "Plugins", CombinedPluginPath, "Source");
    }
}
