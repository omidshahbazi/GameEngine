// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.Utilities;
using System;
using System.Text;

namespace Engine.Frontend.Project.Generator
{
	class MicrosoftSolutionGenerator
	{
		public string Generate(ModuleRules[] Rules)
		{
			StringBuilder builder = new StringBuilder();
			builder.AppendLine("Microsoft Visual Studio Solution File, Format Version 14.00");

			AppendProject(builder, "Engine", "Engine\\Engine.vcxproj");
			AppendProject(builder, "Frontend", "Frontend\\Frontend.csproj");

			builder.AppendLine("Global");

			builder.AppendIndent(1);
			builder.AppendLine("GlobalSection(SolutionConfigurationPlatforms) = preSolution");

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures platform in BuildSystemHelper.PlatformTypes)
					foreach (ModuleRules buildRules in Rules)
						foreach (ModuleRules.BuildRulesBase rule in buildRules.BuildRules)
						{
							if (rule.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
								continue;

							builder.AppendIndent(2);
							builder.AppendLine(string.Format("{0} {1}|{2} = {0} {1}|{2}", configuration, buildRules.Name, BuildSystemHelper.GetPlatformType(platform)));
						}

			builder.AppendIndent(2);
			builder.AppendLine("Debug Frontend|Win32 = Debug Frontend|Win32");
			builder.AppendIndent(2);
			builder.AppendLine("Debug Frontend|x64 = Debug Frontend|x64");

			builder.AppendIndent(2);
			builder.AppendLine("Release Frontend|Win32 = Release Frontend|Win32");
			builder.AppendIndent(2);
			builder.AppendLine("Release Frontend|x64 = Release Frontend|x64");

			builder.AppendIndent(1);
			builder.AppendLine("EndGlobalSection");

			builder.AppendLine("EndGlobal");


			return builder.ToString();
		}

		private static void AppendProject(StringBuilder Builder, string Name, string FilePath)
		{
			Builder.Append("Project(\"");
			Builder.Append(GetNewGUID());
			Builder.Append("\") = \"");
			Builder.Append(Name);
			Builder.Append("\", \"");
			Builder.Append(FilePath);
			Builder.Append("\", \"");
			Builder.Append(GetNewGUID());
			Builder.AppendLine("\"");
			Builder.AppendLine("EndProject");
		}

		private static string GetNewGUID()
		{
			return '{' + Guid.NewGuid().ToString("D").ToUpper() + '}';
		}
	}
}