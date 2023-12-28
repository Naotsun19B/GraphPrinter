// Copyright 2020-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterEditorExtension : ModuleRules
{
	public GraphPrinterEditorExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
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
