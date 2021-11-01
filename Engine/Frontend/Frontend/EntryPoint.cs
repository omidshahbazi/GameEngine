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
					BuildEngine(Arguments);
					break;

				case Actions.RebuildEngine:
					RebuildEngine(Arguments);
					break;

				case Actions.CleanEngine:
					CleanEngine(Arguments);
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

		private static void BuildEngine(ArgumentParser Arguments)
		{
			CreateTargetBuilder(Arguments).Build();
		}

		private static void RebuildEngine(ArgumentParser Arguments)
		{
			CreateTargetBuilder(Arguments).Rebuild();
		}

		private static void CleanEngine(ArgumentParser Arguments)
		{
			CreateTargetBuilder(Arguments).Clean();
		}

		private static void GenerateProjectFile()
		{
			EngineProjectFileCreator.Generate();

			EngineSolutionFileGenerator.Generate();
		}

		private static void GenerateDependencyGraph()
		{
			DependencyGraphGenerator.Generate();
		}

		private static TargetBuilder CreateTargetBuilder(ArgumentParser Arguments)
		{
			if (!Arguments.Contains("Target"))
				ThrowInvalidArgumentsException();

			if (!Arguments.Contains("Configuration"))
				ThrowInvalidArgumentsException();

			if (!Arguments.Contains("Configuration"))
				ThrowInvalidArgumentsException();

			return new TargetBuilder(Arguments.Get<string>("Target"), Arguments.Get<ProjectBase.ProfileBase.BuildConfigurations>("Configuration"), Arguments.Get<ProjectBase.ProfileBase.PlatformArchitectures>("Architecture"));
		}

		private static void ThrowInvalidArgumentsException()
		{
			throw new FrontendException(
				"Invalid arguments" +
				$"\n-Action ({Actions.BuildEngine}|{Actions.RebuildEngine}|{Actions.CleanEngine}|{Actions.BuildScript}|{Actions.GenerateProjectFile}|{Actions.GenerateDependencyGraph})" +
				$"\n-Target (TargetName)" +
				$"\n-Architecture ({ProjectBase.ProfileBase.PlatformArchitectures.x86}|{ProjectBase.ProfileBase.PlatformArchitectures.x64})" +
				$"\n-Configuration ({ProjectBase.ProfileBase.BuildConfigurations.Debug}|{ProjectBase.ProfileBase.BuildConfigurations.Release})");
		}
	}
}
