// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.Utilities;
using System;
using System.Text;

namespace Engine.Frontend.Project.Generator
{
	class MicrosoftSolutionGenerator
	{
		public string Generate(TargetRules[] Targets)
		{
			StringBuilder builder = new StringBuilder();
			builder.AppendLine("Microsoft Visual Studio Solution File, Format Version 14.00");

			AppendProject(builder, "Engine", "Engine.vcxproj");

			builder.AppendLine("Global");

			builder.AppendIndent(1);
			builder.AppendLine("GlobalSection(SolutionConfigurationPlatforms) = preSolution");

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures architecture in BuildSystemHelper.PlatformTypes)
					foreach (TargetRules target in Targets)
					{
						builder.AppendIndent(2);
						builder.AppendLine(string.Format("{0} {1}|{2} = {0} {1}|{2}", configuration, target.ModuleName, MicrosoftVCProjectGenerator.GetPlatformType(architecture)));
					}

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