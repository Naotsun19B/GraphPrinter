// Copyright 2020-2024 Naotsun. All Rights Reserved.

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
        
        // #TODO: Allows the use of details panel printer on Mac/Linux and before UE4.27.
        if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
        {
            if (Target.Version.MajorVersion >= 5)
            {
                PublicDefinitions.Add("WITH_DETAILS_PANEL_PRINTER");
            }
        }
    }
}