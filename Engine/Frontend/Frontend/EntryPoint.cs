// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System;
using Engine.Frontend.System.Generator;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;

namespace Engine.Frontend
{
	static class EntryPoint
	{
		public enum Actions
		{
			BuildEngine = 0,
			RebuildEngine,
			CleanEngine,
			BuildScript,
			GenerateProjectFile,
			GenerateDependencyGraph
		}

		public static void Start(ArgumentParser Arguments)
		{
			if (!Arguments.Contains("Action"))
				ThrowInvalidArgumentsException();

			Actions action = Arguments.Get<Actions>("Action");

			switch (action)
			{
				case Actions.BuildEngine:
					FillBuildVariables(Arguments);
					BuildEngine();
					break;

				case Actions.RebuildEngine:
					FillBuildVariables(Arguments);
					RebuildEngine();
					break;

				case Actions.CleanEngine:
					FillBuildVariables(Arguments);
					CleanEngine();
					break;

				case Actions.BuildScript:
					break;

				case Actions.GenerateProjectFile:
					GenerateProjectFile();
					break;

				case Actions.GenerateDependencyGraph:
					GenerateDependencyGraph();
					break;

				default:
					throw new NotImplementedException($"Handler for {action} has not implemented");
			}
		}

		private static void FillBuildVariables(ArgumentParser Arguments)
		{
			if (!Arguments.Contains("Architecture"))
				ThrowInvalidArgumentsException();

			if (!Arguments.Contains("Configuration"))
				ThrowInvalidArgumentsException();

			BuildSystemHelper.PlatformArchitecture = Arguments.Get<ProjectBase.ProfileBase.PlatformArchitectures>("Architecture");
			BuildSystemHelper.BuildConfiguration = Arguments.Get<ProjectBase.ProfileBase.BuildConfigurations>("Configuration");
		}

		private static void BuildEngine()
		{
			BuildSystem.Instance.Build();
		}

		private static void RebuildEngine()
		{
			BuildSystem.Instance.Rebuild();
		}

		private static void CleanEngine()
		{
			BuildSystem.Instance.Clean();
		}

		private static void GenerateProjectFile()
		{
			BuildSystemHelper.PlatformArchitecture = ProjectBase.ProfileBase.PlatformArchitectures.x64;
			BuildSystemHelper.BuildConfiguration = ProjectBase.ProfileBase.BuildConfigurations.Release;

			EngineProjectFileCreator.Generate();

			EngineSolutionFileGenerator.Generate();
		}

		private static void GenerateDependencyGraph()
		{
			BuildSystemHelper.PlatformArchitecture = ProjectBase.ProfileBase.PlatformArchitectures.x64;
			BuildSystemHelper.BuildConfiguration = ProjectBase.ProfileBase.BuildConfigurations.Release;

			DependencyGraphGenerator.Generate();
		}

		private static void ThrowInvalidArgumentsException()
		{
			throw new FrontendException(
				"Invalid arguments" +
				$"\n-Action ({Actions.BuildEngine}|{Actions.RebuildEngine}|{Actions.CleanEngine}|{Actions.BuildScript}|{Actions.GenerateProjectFile}|{Actions.GenerateDependencyGraph})" +
				$"\n-Architecture ({ProjectBase.ProfileBase.PlatformArchitectures.x86}|{ProjectBase.ProfileBase.PlatformArchitectures.x64})" +
				$"\n-Configuration ({ProjectBase.ProfileBase.BuildConfigurations.Debug}|{ProjectBase.ProfileBase.BuildConfigurations.Release})");
		}
	}
}
