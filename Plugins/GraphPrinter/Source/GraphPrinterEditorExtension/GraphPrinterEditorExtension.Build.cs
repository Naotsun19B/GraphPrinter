// Copyright 2020-2022 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterEditorExtension : ModuleRules
{
	public GraphPrinterEditorExtension(ReadOnlyTargetRules Target) : base(Target)
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
				"InputCore",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"Projects",
				"MainFrame",
				"LevelEditor",

				"GraphPrinterGlobals",
				"WidgetPrinter",
                "TextChunkHelper",
                "ClipboardImageExtension",
			}
		);
	}
}
