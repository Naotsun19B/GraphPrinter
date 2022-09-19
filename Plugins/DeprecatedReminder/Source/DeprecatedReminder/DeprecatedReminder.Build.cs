// Copyright 2022 Naotsun. All Rights Reserved.

using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnrealBuildTool;

public class DeprecatedReminder : ModuleRules
{
	private const string DefaultVerbosity = "Warning";
	private const string IniSectionName = "/Script/DeprecatedReminderEditor.DeprecatedReminderSettings";
	private const string IniKeyName = "DeprecatedRemindVerbosityAtCompileTime";
	private const int IniKeyValueSize = 256;

	[DllImport("kernel32.dll")]
	private static extern uint GetPrivateProfileString(
		string lpApplicationName,
		string lpKeyName,
		string lpDefault,
		StringBuilder lpReturnedString,
		uint nSize,
		string lpFileName
	);
	
	public DeprecatedReminder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);

		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("Projects");
		}

		// Defines macros that compile-time to determine if the expiration date has passed.
		var Now = DateTime.Now;
		PublicDefinitions.AddRange(
			new string[]
			{
				string.Format("DEPRECATED_REMINDER_COMPILE_TIME_YEAR {0} //",   Now.Year  ),
				string.Format("DEPRECATED_REMINDER_COMPILE_TIME_MONTH {0} //",  Now.Month ),
				string.Format("DEPRECATED_REMINDER_COMPILE_TIME_DAY {0} //",    Now.Day   ),
				string.Format("DEPRECATED_REMINDER_COMPILE_TIME_HOUR {0} //",   Now.Hour  ),
				string.Format("DEPRECATED_REMINDER_COMPILE_TIME_MINUTE {0} //", Now.Minute),
			}
		);
		
		// Define a macro that determines what happens when it has expired.
		var Verbosity = new StringBuilder(IniKeyValueSize);
		var Result = GetPrivateProfileString(
			IniSectionName, 
			IniKeyName, 
			DefaultVerbosity,
			Verbosity, 
			Convert.ToUInt32(Verbosity.Capacity),
			DefaultEditorIni
		);
		if (Result == 0)
		{
			Verbosity.Clear();
			Verbosity.Append(DefaultVerbosity);
		}
		
		PublicDefinitions.AddRange(
			new string[]
			{
				string.Format("DEPRECATED_REMINDER_VERBOSITY_ERROR {0} //",   Convert.ToInt32(Verbosity.ToString() == "Error")  ),
				string.Format("DEPRECATED_REMINDER_VERBOSITY_WARNING {0} //", Convert.ToInt32(Verbosity.ToString() == "Warning")),
			}
		);

		Console.WriteLine("[Deprecated Reminder] Compile Time : {0:g} | Verbosity : {1}", Now, Verbosity);
	}

	private string ProjectDirectory
	{
		get
		{
			foreach (var CommandLineArg in Environment.GetCommandLineArgs())
			{
				var NameAndValues = new List<string>(CommandLineArg.Split('='));
				if (NameAndValues.Count < 2)
				{
					continue;
				}
				if (NameAndValues[0] != "-Project")
				{
					continue;
				}
				NameAndValues.RemoveAt(0);
				var UProjectFilePath = string.Join("", NameAndValues.ToArray());
				return Path.GetDirectoryName(UProjectFilePath);
			}

			return null;
		}
	}

	private string DefaultEditorIni
	{
		get
		{
			return Path.Combine(ProjectDirectory, "Config", "DefaultEditor.ini");
		}
	}
}
// 2022.07.18-12.39.03
