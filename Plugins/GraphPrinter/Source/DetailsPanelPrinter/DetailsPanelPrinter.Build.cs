// Copyright 2020-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class DetailsPanelPrinter : ModuleRules
{
    public DetailsPanelPrinter(ReadOnlyTargetRules Target) : base(Target)
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
                "UMG",
                "Slate",
                "SlateCore",
                "Engine",
                "UnrealEd",
                "MainFrame",
                "RenderCore",

                "GraphPrinterGlobals",
                "WidgetPrinter",
                "TextChunkHelper",
                "ClipboardImageExtension",
            }
        );
    }
}