// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System
{
	class BuildSystem
	{
		public enum Actions
		{
			BuildEngine = 0,
			BuildScript,
			BuildProjectFile
		}

		public enum PlatformArchitectures
		{
			x86 = 0,
			x64
		}

		private static Dictionary<string, SourceBuilder> sourceBuilders = new Dictionary<string, SourceBuilder>();

		private Compiler compiler = new Compiler();

		public static bool GenerateReflection
		{
			get;
			private set;
		}

		public static ProjectBase.ProfileBase.PlatformTypes PlatformType
		{
			get;
			private set;
		}

		public static ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration
		{
			get;
			private set;
		}

		public BuildSystem(Actions Action, PlatformArchitectures PlatformArchitecture, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration)
		{
			ConsoleHelper.WriteLineInfo(EnvironmentHelper.ManagedRuntime + " under " + EnvironmentHelper.Platform + " is present");

			PlatformType = (PlatformArchitecture == PlatformArchitectures.x86 ? ProjectBase.ProfileBase.PlatformTypes.x86 : ProjectBase.ProfileBase.PlatformTypes.x64);
			BuildSystem.BuildConfiguration = BuildConfiguration;

			compiler.ErrorRaised += OnError;
		}

		public bool Build()
		{
			RuleLibraryBuilder rulesBuilder = new RuleLibraryBuilder(EnvironmentHelper.ProcessDirectory);
			if (!rulesBuilder.Build())
				return false;

			for (int i = 0; i < rulesBuilder.Rules.Length; ++i)
			{
				BuildRules rules = rulesBuilder.Rules[i];
				sourceBuilders[rules.ModuleName] = new SourceBuilder(rules, Path.GetDirectoryName(rulesBuilder.RulesFiles[i]) + EnvironmentHelper.PathSeparator);
			}

			BuildSources();

			return true;
		}

		private void BuildSources()
		{
			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteLineInfo("Building source starts at " + startTime.ToString());

			for (BuildRules.Priorities priority = BuildRules.Priorities.PreBuildProcess; priority <= BuildRules.Priorities.PostBuildProcess; priority++)
			{
				GenerateReflection = (priority != BuildRules.Priorities.PreBuildProcess);

				foreach (SourceBuilder builder in sourceBuilders.Values)
					if (builder.SelectedRule.Priority == priority)
						BuildSourceBuilder(builder);
			}

			ConsoleHelper.WriteLineInfo("Building source takes " + (DateTime.Now - startTime).ToHHMMSS());
		}

		private bool BuildSourceBuilder(SourceBuilder Builder)
		{
            bool forceToRebuild = false;

			if (Builder.SelectedRule.DependencyModulesName != null)
				foreach (string dep in Builder.SelectedRule.DependencyModulesName)
				{
					if (!sourceBuilders.ContainsKey(dep))
					{
						ConsoleHelper.WriteLineWarning("Dependency [" + dep + "] doesn't exists");
						continue;
					}

					SourceBuilder builder = sourceBuilders[dep];

					if (builder == null)
						return false;

					if (!BuildSourceBuilder(builder))
						return false;

                    if (builder.State == SourceBuilder.States.Built)
                        forceToRebuild = true;
                }

			return Builder.Build(forceToRebuild);
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteLineError(Text);
		}

		public static SourceBuilder GetSourceBuilder(string Name)
		{
			if (!sourceBuilders.ContainsKey(Name))
				return null;

			return sourceBuilders[Name];
		}
	}
}
