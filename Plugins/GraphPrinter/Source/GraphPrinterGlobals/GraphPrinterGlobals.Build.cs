// Copyright 2020-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphPrinterGlobals : ModuleRules
{
	public GraphPrinterGlobals(ReadOnlyTargetRules Target) : base(Target)
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
				"Slate",
				"SlateCore",
				"Settings",
				"SettingsEditor",
				"DesktopPlatform",
				"ImageWriteQueue",
			}
		);
	}
}
