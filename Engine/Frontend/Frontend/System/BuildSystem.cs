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
		private static readonly BuildSystem instance = new BuildSystem();

		private List<ModuleRules> modules = null;
		private Dictionary<string, EngineBuilder> engineBuilders = null;

		public static BuildSystem Instance
		{
			get { return instance; }
		}

		private BuildSystem()
		{
			ConsoleHelper.WriteInfo($"{EnvironmentHelper.ManagedRuntime} under {EnvironmentHelper.Platform} is present");

			modules = new List<ModuleRules>();
			engineBuilders = new Dictionary<string, EngineBuilder>();
			RuleLibraryBuilder.Instance.OnNewModuleRule += (ModuleRules Module) =>
				{
					EngineBuilder builder = new EngineBuilder(Module);
					builder.Initialize();

					engineBuilders[Module.Name] = builder;

					modules.Add(Module);
				};

			RuleLibraryBuilder.Instance.Build(true);
		}

		public void Build()
		{
			BuildInternal();
		}

		public void Rebuild()
		{
			CleanInternal();

			BuildInternal();
		}

		public void Clean()
		{
			CleanInternal();
		}

		public EngineBuilder GetEngineBuilder(string Name)
		{
			if (!engineBuilders.ContainsKey(Name))
				throw new FrontendException($"Dependency {Name} doesn't exists");

			return engineBuilders[Name];
		}

		private void BuildInternal()
		{
			CheckCircularDependencies();

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building engine starts");

			for (ModuleRules.Priorities priority = ModuleRules.Priorities.PreBuildProcess; priority <= ModuleRules.Priorities.PostBuildProcess; priority++)
				foreach (EngineBuilder builder in engineBuilders.Values)
					if (builder.BuildRules.Priority == priority)
						BuildEngineBuilder(builder);

			ConsoleHelper.WriteInfo($"Building rules took {(DateTime.Now - startTime).ToHHMMSS()}");
		}

		private void CleanInternal()
		{
			foreach (ModuleRules module in modules)
			{
				string intermediateDirectory = EnvironmentHelper.IntermediateDirectory + module.Name;

				if (Directory.Exists(intermediateDirectory))
					Directory.Delete(intermediateDirectory, true);
			}

			if (Directory.Exists(EnvironmentHelper.OutputDirectory))
				Directory.Delete(EnvironmentHelper.OutputDirectory, true);
		}

		private void BuildEngineBuilder(EngineBuilder Builder)
		{
			string[] dependencies = Builder.BuildRules.GetAllDependencies();

			bool forceToRebuild = false;
			foreach (string dependency in dependencies)
			{
				EngineBuilder builder = GetEngineBuilder(dependency);

				BuildEngineBuilder(builder);

				if (builder.State != BuilderBase.States.Built)
					forceToRebuild = true;
			}

			Builder.Build(forceToRebuild);
		}

		private void CheckCircularDependencies()
		{
			foreach (EngineBuilder builder in engineBuilders.Values)
			{
				Stack<EngineBuilder> stack = new Stack<EngineBuilder>();
				CheckCircularDependencies(builder, stack);
			}
		}

		private void CheckCircularDependencies(EngineBuilder Builder, Stack<EngineBuilder> BuilderStack)
		{
			BuilderStack.Push(Builder);

			if (Builder.BuildRules.PrivateDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PrivateDependencyModuleNames)
				{
					EngineBuilder builder = GetEngineBuilder(dep);

					if (BuilderStack.Contains(builder))
						throw new FrontendException($"A circular dependency between {Builder.Module.Name} and {builder.Module.Name} has detected");
				}

			if (Builder.BuildRules.PublicDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PublicDependencyModuleNames)
				{
					EngineBuilder builder = GetEngineBuilder(dep);

					if (BuilderStack.Contains(builder))
						throw new FrontendException($"A circular dependency between {Builder.Module.Name} and {builder.Module.Name} has detected");

					CheckCircularDependencies(builder, BuilderStack);
				}

			BuilderStack.Pop();
		}
	}
}