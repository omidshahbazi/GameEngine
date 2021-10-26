// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project.Generator;
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
			RulesLibrary.Instance.Build(false);

			File.WriteAllText(ProjectFilePath, new MicrosoftSolutionGenerator().Generate(RulesLibrary.Instance.TargetRules));

			return true;
		}
	}
}