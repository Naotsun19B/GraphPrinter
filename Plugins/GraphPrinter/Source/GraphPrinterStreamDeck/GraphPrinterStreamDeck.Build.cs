// Copyright 2020-2024 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GraphPrinterStreamDeck : ModuleRules
{
    public GraphPrinterStreamDeck(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
        
        PublicIncludePaths.AddRange(
            new string[] 
            {
                Path.Combine(ModuleDirectory, "Public", "GraphPrinterStreamDeck"),
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
                "Projects",
                
                "GraphPrinterGlobals",
            }
        );
        
        // #TODO: Allows the use of the Stream Deck plugin on Mac.
        if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
        {
            PublicDefinitions.Add("WITH_STREAM_DECK");
        }
    }
}