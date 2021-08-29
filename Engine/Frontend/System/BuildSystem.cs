// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			BuildProjectFile,
			BuildDependencyGraph
		}

		private List<ModuleRules> modules = null;

		private static Dictionary<string, EngineBuilder> engineBuilders = null;

		public BuildSystem()
		{
			ConsoleHelper.WriteInfo(EnvironmentHelper.ManagedRuntime + " under " + EnvironmentHelper.Platform + " is present");

			engineBuilders = new Dictionary<string, EngineBuilder>();

			RuleLibraryBuilder.Instance.OnNewModuleRule += RuleLibraryBuilder_OnNewBuildRule;

			modules = new List<ModuleRules>();
		}

		private void RuleLibraryBuilder_OnNewBuildRule(ModuleRules Module)
		{
			EngineBuilder builder = new EngineBuilder(Module, Module.GetSourceRootDirectory() + EnvironmentHelper.PathSeparator);
			builder.Initialize();

			engineBuilders[Module.Name] = builder;

			modules.Add(Module);
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
			foreach (ModuleRules module in modules)
			{
				string intermediateDirectory = EnvironmentHelper.IntermediateDirectory + module.Name;

				if (Directory.Exists(intermediateDirectory))
					Directory.Delete(intermediateDirectory, true);
			}

			if (Directory.Exists(EnvironmentHelper.OutputDirectory))
				Directory.Delete(EnvironmentHelper.OutputDirectory, true);

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
			for (ModuleRules.Priorities priority = ModuleRules.Priorities.PreBuildProcess; priority <= ModuleRules.Priorities.PostBuildProcess; priority++)
			{
				foreach (EngineBuilder builder in engineBuilders.Values)
					if (builder.BuildRules.Priority == priority)
						if (!BuildEngineBuilder(builder))
							areBuildersOK = false;
			}

			ConsoleHelper.WriteInfo("Building engine takes " + (DateTime.Now - startTime).ToHHMMSS());

			return areBuildersOK;
		}

		private static bool CheckForCircularDependencies(EngineBuilder Builder, Stack<EngineBuilder> BuilderStack)
		{
			BuilderStack.Push(Builder);

			if (Builder.BuildRules.PrivateDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PrivateDependencyModuleNames)
				{
					EngineBuilder builder = engineBuilders[dep];

					if (BuilderStack.Contains(builder))
					{
						ConsoleHelper.WriteError("A circular dependency between [" + Builder.Module.Name + "] and [" + builder.Module.Name + "] was detected");
						return false;
					}
				}

			if (Builder.BuildRules.PublicDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PublicDependencyModuleNames)
				{
					EngineBuilder builder = engineBuilders[dep];

					if (BuilderStack.Contains(builder))
					{
						ConsoleHelper.WriteError("A circular dependency between [" + Builder.Module.Name + "] and [" + builder.Module.Name + "] was detected");
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

			List<string> dependencies = new List<string>();
			if (Builder.BuildRules.PrivateDependencyModuleNames != null)
				dependencies.AddRange(Builder.BuildRules.PrivateDependencyModuleNames);
			if (Builder.BuildRules.PublicDependencyModuleNames != null)
				dependencies.AddRange(Builder.BuildRules.PublicDependencyModuleNames);

			foreach (string dep in dependencies)
			{
				if (!engineBuilders.ContainsKey(dep))
				{
					ConsoleHelper.WriteWarning("Dependency [" + dep + "] doesn't exists");
					continue;
				}

				EngineBuilder builder = engineBuilders[dep];

				if (!BuildEngineBuilder(builder))
					return false;

				if (builder.State != EngineBuilder.States.Built)
					forceToRebuild = true;
			}

			return Builder.Build(forceToRebuild);
		}

		public static EngineBuilder GetEngineBuilder(string Name)
		{
			if (!engineBuilders.ContainsKey(Name))
				return null;

			return engineBuilders[Name];
		}
	}
}
