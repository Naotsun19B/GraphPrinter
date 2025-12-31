// Copyright 2020-2026 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DetailsPanelPrinter : ModuleRules
{
    public DetailsPanelPrinter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
#if !UE_5_0_OR_LATER
        CppStandard = CppStandardVersion.Cpp17;
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
                "UMG",
                "Slate",
                "SlateCore",
                "Engine",
                "UnrealEd",
                "MainFrame",
                "Json",
                "JsonUtilities",

                "GraphPrinterGlobals",
                "WidgetPrinter",
                "TextChunkHelper",
                "ClipboardImageExtension",
            }
        );
        if (Target.Version.MajorVersion >= 5)
        {
            PrivateDependencyModuleNames.Add("SubobjectEditor");
        }
        
        PublicIncludePaths.AddRange(
            new string[]
            {
#if UE_5_2_OR_LATER
                // In UHT for UE5.2 and later, when you use PublicIncludePaths, the base path of the module's include will be replaced with the first value, so add the correct base path of the include first.
                Path.Combine(ModuleDirectory, ".."),
#endif
                // To use SDetailsView.
                Path.Combine(EngineDirectory, "Source", "Editor", "PropertyEditor", "Private"),

                // To use SActorDetails.
                Path.Combine(EngineDirectory, "Source", "Editor", "LevelEditor", "Private"),
            }
        );
    }
}