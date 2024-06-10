// Copyright 2020-2024 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GenericGraphPrinter : ModuleRules
{
	public GenericGraphPrinter(ReadOnlyTargetRules Target) : base(Target)
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
#if UE_5_2_OR_LATER
				// In UHT for UE5.2 and later, when you use PublicIncludePaths, the base path of the module's include will be replaced with the first value, so add the correct base path of the include first.
				Path.Combine(ModuleDirectory, ".."),
#endif
				// To use SGraphEditorImpl.
				Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
			}
		);
	}
}
