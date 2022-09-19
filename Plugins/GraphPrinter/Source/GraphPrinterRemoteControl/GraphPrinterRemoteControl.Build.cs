// Copyright 2020-2022 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterRemoteControl : ModuleRules
{
    public GraphPrinterRemoteControl(ReadOnlyTargetRules Target) : base(Target)
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
                "WebSockets",
                
                "GraphPrinterGlobals",
                "GraphPrinterEditorExtension",
                "ClipboardImageExtension",
                "TextChunkHelper",
            }
        );
    }
}