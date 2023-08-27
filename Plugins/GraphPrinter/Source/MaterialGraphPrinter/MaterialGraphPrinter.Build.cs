// Copyright 2020-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class MaterialGraphPrinter : ModuleRules
{
	public MaterialGraphPrinter(ReadOnlyTargetRules Target) : base(Target)
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
				"UMG",
				"Slate",
				"SlateCore",
				"Engine",
				"UnrealEd",
				"MainFrame",
				"RenderCore",

				"GraphPrinterGlobals",
				"WidgetPrinter",
				"GenericGraphPrinter",
				"PreviewViewportPrinter",
				"TextChunkHelper",
				"ClipboardImageExtension",
			}
		);
	}
}
