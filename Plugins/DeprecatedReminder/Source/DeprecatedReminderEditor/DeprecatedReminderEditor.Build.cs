// Copyright 2022 Naotsun. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class DeprecatedReminderEditor : ModuleRules
{
	public DeprecatedReminderEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"Settings",
				"Projects",
			}
		);
	}
}
