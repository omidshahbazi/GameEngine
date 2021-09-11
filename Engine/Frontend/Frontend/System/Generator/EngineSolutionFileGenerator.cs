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
			get { return EnvironmentHelper.EngineDirectory + "Engine.sln"; }
		}

		public static bool Generate()
		{
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;

			List<TargetRules> targets = new List<TargetRules>();

			NewTargetRuleEventHandler newRuleCallback = (rule) =>
			{
				targets.Add(rule);
			};

			rulesBuilder.OnNewTargetRule += newRuleCallback;

			rulesBuilder.Build(false);

			File.WriteAllText(ProjectFilePath, new MicrosoftSolutionGenerator().Generate(targets.ToArray()));

			rulesBuilder.OnNewTargetRule -= newRuleCallback;

			return true;
		}
	}
}