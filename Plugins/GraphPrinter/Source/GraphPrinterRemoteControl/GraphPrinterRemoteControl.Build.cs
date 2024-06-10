// Copyright 2020-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterRemoteControl : ModuleRules
{
    public GraphPrinterRemoteControl(ReadOnlyTargetRules Target) : base(Target)
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