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
	class TargetBuilder
	{
		private TargetRules targetRules = null;
		private List<ModuleBuilder> moduleBuilders = null;

		public ProjectBase.ProfileBase.BuildConfigurations Configuration
		{
			get;
			private set;
		}

		public ProjectBase.ProfileBase.PlatformArchitectures Architecture
		{
			get;
			private set;
		}

		public TargetBuilder(string Target, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ConsoleHelper.WriteInfo($"{EnvironmentHelper.ManagedRuntime} under {EnvironmentHelper.OperatingSystem} is present");

			moduleBuilders = new List<ModuleBuilder>();
			this.Configuration = Configuration;
			this.Architecture = Architecture;

			RulesLibrary.Instance.Build(true);

			targetRules = RulesLibrary.Instance.GetTargetRules(Target, Configuration, Architecture);

			FillModuleBuilders(targetRules);
		}

		public ModuleBuilder GetModuleBuilder(string Name)
		{
			ModuleBuilder builder = moduleBuilders.Find((ModuleBuilder item) => item.Module.Name == Name);
			if (builder != null)
				return builder;

			throw new FrontendException($"Couldn't find {Name} module rules");
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

		private void FillModuleBuilders(TargetRules Target)
		{
			FillModuleBuilders(Target.ModuleName);

			foreach (string module in Target.RequiredModuleNames)
				FillModuleBuilders(module);
		}

		private void FillModuleBuilders(string Name)
		{
			ModuleRules module = RulesLibrary.Instance.GetModuleRules(Name, Configuration, Architecture);

			if (moduleBuilders.Find((ModuleBuilder item) => item.Module == module) != null)
				return;

			ModuleBuilder builder = new ModuleBuilder(this, module);

			string[] dependencies = module.GetAllDependencies();
			foreach (string dependency in dependencies)
				FillModuleBuilders(dependency);

			if (module.GenerateReflection)
				FillModuleBuilders(EnvironmentHelper.ReflectionToolModuleName);

			moduleBuilders.Add(builder);
		}

		private void BuildInternal()
		{
			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo($"Building {targetRules.ModuleName} {Configuration} {Architecture.GetPrettyName()} starts");

			bool forceToRebuild = false;
			foreach (ModuleBuilder builder in moduleBuilders)
			{
				builder.Build(forceToRebuild);

				if (builder.State == BaseBuilder.States.Failed)
					return;

				if (builder.State != BaseBuilder.States.Built)
					forceToRebuild = true;
			}

			ConsoleHelper.WriteInfo($"Building rules took {(DateTime.Now - startTime).ToHHMMSS()}");
		}

		private void CleanInternal()
		{
			ModuleRules[] moduleRules = RulesLibrary.Instance.GetModuleRules(Configuration, Architecture);

			foreach (ModuleRules module in moduleRules)
			{
				string intermediateDirectory = module.Name.GetIntermediateOutputPath(Configuration, Architecture);

				if (Directory.Exists(intermediateDirectory))
					Directory.Delete(intermediateDirectory, true);
			}

			string outputDirectory = EnvironmentHelper.GetOutputDirectory(Configuration, Architecture);
			if (Directory.Exists(outputDirectory))
				Directory.Delete(outputDirectory, true);
		}
	}
}
