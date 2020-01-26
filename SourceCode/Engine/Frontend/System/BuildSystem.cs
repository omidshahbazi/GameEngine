// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
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
			RebuildEngine,
			CleanEngine,
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
			ConsoleHelper.WriteInfo(EnvironmentHelper.ManagedRuntime + " under " + EnvironmentHelper.Platform + " is present");

			PlatformType = (PlatformArchitecture == PlatformArchitectures.x86 ? ProjectBase.ProfileBase.PlatformTypes.x86 : ProjectBase.ProfileBase.PlatformTypes.x64);
			BuildSystem.BuildConfiguration = BuildConfiguration;

			compiler.ErrorRaised += OnError;
		}

		public bool Build()
		{
			RuleLibraryBuilder rules = BuildRulesLibrary();
			if (rules == null)
				return false;

			BuildInternal(rules);

			return true;
		}

		public bool Rebuild()
		{
			RuleLibraryBuilder rules = BuildRulesLibrary();
			if (rules == null)
				return false;

			CleanInternal(rules);
			BuildInternal(rules);

			return true;
		}

		public bool Clean()
		{
			RuleLibraryBuilder rules = BuildRulesLibrary();
			if (rules == null)
				return false;

			CleanInternal(rules);

			return true;
		}

		private void BuildInternal(RuleLibraryBuilder Rules)
		{
			List<string> wrapperCSFiles = new List<string>();

			for (int i = 0; i < Rules.Rules.Length; ++i)
			{
				BuildRules rule = Rules.Rules[i];
				sourceBuilders[rule.ModuleName] = new SourceBuilder(rule, Path.GetDirectoryName(Rules.RulesFiles[i]) + EnvironmentHelper.PathSeparator, wrapperCSFiles);
			}

			BuildSources();

			BuildWrapperLibrary(wrapperCSFiles);
		}

		private void CleanInternal(RuleLibraryBuilder Rules)
		{
			foreach (BuildRules rule in Rules.Rules)
			{
				string intermediatePath = EnvironmentHelper.IntermediateDirectory + rule.ModuleName;

				if (Directory.Exists(intermediatePath))
					Directory.Delete(intermediatePath, true);

				foreach (BuildRules.RuleBase ruleBase in rule.Rules)
				{
					string outputFilePath = EnvironmentHelper.FinalOutputDirectory + ruleBase.TargetName + (ruleBase.LibraryUseType == BuildRules.LibraryUseTypes.Executable ? EnvironmentHelper.ExecutableExtentions : EnvironmentHelper.DynamicLibraryExtentions);

					if (File.Exists(outputFilePath))
						File.Delete(outputFilePath);
				}
			}
		}

		private RuleLibraryBuilder BuildRulesLibrary()
		{
			RuleLibraryBuilder rulesBuilder = new RuleLibraryBuilder(EnvironmentHelper.ProcessDirectory);

			if (!rulesBuilder.Build())
				return null;

			return rulesBuilder;
		}

		private bool BuildWrapperLibrary(List<string> WrapperFiles)
		{
			const string ProjectName = "Wrapper";

			string projectDir = EnvironmentHelper.IntermediateDirectory + ProjectName + EnvironmentHelper.PathSeparator;

			if (!Directory.Exists(projectDir))
				Directory.CreateDirectory(projectDir);

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = projectDir + "Build" + EnvironmentHelper.PathSeparator;
			profile.IntermediatePath = projectDir;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building wrapper starts at " + startTime.ToString());

			if (WrapperFiles.Count == 0)
			{
				ConsoleHelper.WriteInfo("No building rules found, aborting process");
				return false;
			}

			foreach (string file in WrapperFiles)
				csproj.AddCompileFile(file);

			if (compiler.Build(profile))
				return true;

			ConsoleHelper.WriteInfo("Building wrapper takes " + (DateTime.Now - startTime).ToHHMMSS());

			return false;
		}

		private void BuildSources()
		{
			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building source starts at " + startTime.ToString());

			for (BuildRules.Priorities priority = BuildRules.Priorities.PreBuildProcess; priority <= BuildRules.Priorities.PostBuildProcess; priority++)
			{
				foreach (SourceBuilder builder in sourceBuilders.Values)
					if (builder.SelectedRule.Priority == priority)
						BuildSourceBuilder(builder);
			}

			ConsoleHelper.WriteInfo("Building source takes " + (DateTime.Now - startTime).ToHHMMSS());
		}

		private bool BuildSourceBuilder(SourceBuilder Builder)
		{
			bool forceToRebuild = false;

			if (Builder.SelectedRule.DependencyModulesName != null)
				foreach (string dep in Builder.SelectedRule.DependencyModulesName)
				{
					if (!sourceBuilders.ContainsKey(dep))
					{
						ConsoleHelper.WriteWarning("Dependency [" + dep + "] doesn't exists");
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
			ConsoleHelper.WriteError(Text);
		}

		public static SourceBuilder GetSourceBuilder(string Name)
		{
			if (!sourceBuilders.ContainsKey(Name))
				return null;

			return sourceBuilders[Name];
		}
	}
}
