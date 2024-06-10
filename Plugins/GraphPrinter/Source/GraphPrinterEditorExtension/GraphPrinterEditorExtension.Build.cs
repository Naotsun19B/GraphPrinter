// Copyright 2020-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterEditorExtension : ModuleRules
{
	public GraphPrinterEditorExtension(ReadOnlyTargetRules Target) : base(Target)
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
				"InputCore",
				"UnrealEd",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"Projects",
				"MainFrame",
				"ToolMenus",

				"GraphPrinterGlobals",
				"WidgetPrinter",
                "TextChunkHelper",
                "ClipboardImageExtension",
                "GraphPrinterStreamDeck",
			}
		);
	}
}
