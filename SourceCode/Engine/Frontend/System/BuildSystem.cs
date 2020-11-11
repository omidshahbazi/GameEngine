// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
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

		//private WrapperLibraryBuilder wrapperLibraryBuilder = null;
		private List<BuildRules> rules = null;

		private static Dictionary<string, EngineBuilder> engineBuilders = null;

		public static ProjectBase.ProfileBase.PlatformArchitectures PlatformArchitecture
		{
			get;
			private set;
		}

		public static ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration
		{
			get;
			private set;
		}

		public BuildSystem(ProjectBase.ProfileBase.PlatformArchitectures PlatformArchitecture, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration)
		{
			ConsoleHelper.WriteInfo(EnvironmentHelper.ManagedRuntime + " under " + EnvironmentHelper.Platform + " is present");

			BuildSystem.PlatformArchitecture = PlatformArchitecture;
			BuildSystem.BuildConfiguration = BuildConfiguration;
			engineBuilders = new Dictionary<string, EngineBuilder>();

			RuleLibraryBuilder.Instance.OnNewBuildRule += RuleLibraryBuilder_OnNewBuildRule;

			//wrapperLibraryBuilder = new WrapperLibraryBuilder();

			rules = new List<BuildRules>();
		}

		private void RuleLibraryBuilder_OnNewBuildRule(string FilePath, BuildRules Rule)
		{
			EngineBuilder builder = new EngineBuilder(Rule, Path.GetDirectoryName(FilePath) + EnvironmentHelper.PathSeparator);
			builder.OnNewWrapperFile += Builder_OnNewWrapperFile;

			engineBuilders[Rule.ModuleName] = builder;

			rules.Add(Rule);
		}

		private void Builder_OnNewWrapperFile(string FilePath)
		{
			//wrapperLibraryBuilder.AddWrapperFile(FilePath);
		}

		public bool Build()
		{
			if (!RuleLibraryBuilder.Instance.Build(true))
				return false;

			if (!BuildInternal())
				return false;

			return true;
		}

		public bool Rebuild()
		{
			if (!RuleLibraryBuilder.Instance.Build(true))
				return false;

			if (!CleanInternal())
				return false;

			if (!BuildInternal())
				return false;

			return true;
		}

		public bool Clean()
		{
			if (!RuleLibraryBuilder.Instance.Build(true))
				return false;

			return CleanInternal();
		}

		private bool BuildInternal()
		{
			if (!BuildEngineBuilders())
				return false;

			return true;
		}

		private bool CleanInternal()
		{
			foreach (BuildRules rule in rules)
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

			return true;
		}

		private bool BuildEngineBuilders()
		{
			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building engine starts at " + startTime.ToString());

			bool areDependenciesOK = true;
			foreach (EngineBuilder builder in engineBuilders.Values)
			{
				Stack<EngineBuilder> stack = new Stack<EngineBuilder>();
				if (!CheckForCircularDependencies(builder, stack))
					areDependenciesOK = false;
			}

			if (!areDependenciesOK)
				return false;

			bool areBuildersOK = true;
			for (BuildRules.Priorities priority = BuildRules.Priorities.PreBuildProcess; priority <= BuildRules.Priorities.PostBuildProcess; priority++)
			{
				foreach (EngineBuilder builder in engineBuilders.Values)
					if (builder.SelectedRule.Priority == priority)
						if (!BuildEngineBuilder(builder))
							areBuildersOK = false;
			}

			ConsoleHelper.WriteInfo("Building engine takes " + (DateTime.Now - startTime).ToHHMMSS());

			return areBuildersOK;
		}

		private static bool CheckForCircularDependencies(EngineBuilder Builder, Stack<EngineBuilder> BuilderStack)
		{
			BuilderStack.Push(Builder);

			if (Builder.SelectedRule.DependencyModulesName != null)
				foreach (string dep in Builder.SelectedRule.DependencyModulesName)
				{
					EngineBuilder builder = engineBuilders[dep];

					if (BuilderStack.Contains(builder))
					{
						ConsoleHelper.WriteError("A circular dependency between [" + Builder.BuildRule.ModuleName + "] and [" + builder.BuildRule.ModuleName + "] was detected");
						return false;
					}

					if (!CheckForCircularDependencies(builder, BuilderStack))
						return false;
				}

			BuilderStack.Pop();

			return true;
		}

		private bool BuildEngineBuilder(EngineBuilder Builder)
		{
			bool forceToRebuild = false;

			if (Builder.SelectedRule.DependencyModulesName != null)
				foreach (string dep in Builder.SelectedRule.DependencyModulesName)
				{
					if (!engineBuilders.ContainsKey(dep))
					{
						ConsoleHelper.WriteWarning("Dependency [" + dep + "] doesn't exists");
						continue;
					}

					EngineBuilder builder = engineBuilders[dep];

					if (!BuildEngineBuilder(builder))
						return false;

					if (builder.State == EngineBuilder.States.Built)
						forceToRebuild = true;
				}

			return Builder.Build(forceToRebuild);
		}

		//private bool BuildWrapperLibrary()
		//{
		//	return wrapperLibraryBuilder.Build(true);
		//}

		public static EngineBuilder GetEngineBuilder(string Name)
		{
			if (!engineBuilders.ContainsKey(Name))
				return null;

			return engineBuilders[Name];
		}
	}
}
