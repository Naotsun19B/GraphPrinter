// Copyright 2020-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class PreviewViewportPrinter : ModuleRules
{
	public PreviewViewportPrinter(ReadOnlyTargetRules Target) : base(Target)
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
				
				// #TODO: Need a graph editor to get the asset name in the preview viewport.
				"GenericGraphPrinter"
			}
		);
	}
}
