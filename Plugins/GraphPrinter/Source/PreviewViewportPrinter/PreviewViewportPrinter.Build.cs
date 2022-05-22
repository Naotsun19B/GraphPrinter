// Copyright 2020-2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PreviewViewportPrinter : ModuleRules
{
	public PreviewViewportPrinter(ReadOnlyTargetRules Target) : base(Target)
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
				"InputCore",
				"ImageWriteQueue",
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
		
        PublicIncludePaths.AddRange(
            new string[]
            {
	            // To use SGraphEditorImpl.
	            Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
				
	            // To use SDockingTabStack.
	            Path.Combine(EngineDirectory, "Source", "Runtime", "Slate", "Private"),
	            
	            // To use SStandaloneAssetEditorToolkitHost.
	            Path.Combine(EngineDirectory, "Source", "Editor", "UnrealEd", "Private"),
            }
        );
	}
}
