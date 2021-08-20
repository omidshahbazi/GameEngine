// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Build;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Generator
{
	static class EngineSolutionFileGenerator
	{
		private static string ProjectFilePath
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine.sln"; }
		}

		public static bool Generate()
		{
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;

			List<ModuleRules> rules = new List<ModuleRules>();

			NewBuildRuleEventHandler newRuleCallback = (filePath, rule) =>
			{
				rules.Add(rule);
			};

			rulesBuilder.OnNewBuildRule += newRuleCallback;

			if (!rulesBuilder.Build(false))
			{
				rulesBuilder.OnNewBuildRule -= newRuleCallback;
				return false;
			}

			rulesBuilder.OnNewBuildRule -= newRuleCallback;

			MicrosoftSolutionGenerator generator = new MicrosoftSolutionGenerator();

			File.WriteAllText(ProjectFilePath, generator.Generate(rules.ToArray()));

			return true;
		}
	}
}