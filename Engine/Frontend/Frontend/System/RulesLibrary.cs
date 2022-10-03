// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;

namespace Engine.Frontend.System
{
	class RulesLibrary
	{
		private class RulesRepository<T> where T : BaseRules
		{
			private class RulesList : List<T>
			{
			}

			private class ArchitectureMap : Dictionary<ProjectBase.ProfileBase.PlatformArchitectures, RulesList>
			{
			}

			private class ConfigurationMap : Dictionary<ProjectBase.ProfileBase.BuildConfigurations, ArchitectureMap>
			{
			}

			private ConfigurationMap rules = null;

			public RulesRepository()
			{
				rules = new ConfigurationMap();
			}

			public void Add(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture, T Rules)
			{
				ArchitectureMap architectureMap = null;
				if (rules.ContainsKey(Configuration))
					architectureMap = rules[Configuration];
				else
					architectureMap = rules[Configuration] = new ArchitectureMap();

				RulesList list = null;
				if (architectureMap.ContainsKey(Architecture))
					list = architectureMap[Architecture];
				else
					list = architectureMap[Architecture] = new RulesList();

				list.Add(Rules);
			}

			public void Clear()
			{
				rules.Clear();
			}

			public T[] Get(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
			{
				if (!rules.ContainsKey(Configuration))
					throw new NotImplementedException($"Handler for {Configuration} has not implemented");

				ArchitectureMap architectureMap = rules[Configuration];

				if (!architectureMap.ContainsKey(Architecture))
					throw new NotImplementedException($"Handler for {Architecture} has not implemented");

				return architectureMap[Architecture].ToArray();
			}
		}

		private class ModuleRulesList : List<ModuleRules>
		{
		}

		private class PlatformMap : Dictionary<ProjectBase.ProfileBase.PlatformArchitectures, ModuleRulesList>
		{
		}

		private class ConfigurationMap : Dictionary<ProjectBase.ProfileBase.BuildConfigurations, PlatformMap>
		{
		}

		private static readonly RulesLibrary instance = new RulesLibrary();

		private RulesLibraryBuilder builder = null;

		private RulesRepository<ModuleRules> moduleRules = null;
		private RulesRepository<TargetRules> targetRules = null;

		public static RulesLibrary Instance
		{
			get { return instance; }
		}

		private RulesLibrary()
		{
			builder = new RulesLibraryBuilder(ProjectBase.ProfileBase.BuildConfigurations.Release, ProjectBase.ProfileBase.PlatformArchitectures.x64);

			moduleRules = new RulesRepository<ModuleRules>();
			targetRules = new RulesRepository<TargetRules>();
		}

		public void Build(bool ForceToRebuild, params string[] AdditionalPreprocessors)
		{
			if (!BuildInternal(ForceToRebuild, AdditionalPreprocessors))
				return;

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures architecture in BuildSystemHelper.PlatformTypes)
				{
					LoadModules(configuration, architecture);

					CheckCircularDependencies(configuration, architecture);

					LoadTargets(configuration, architecture);
				}
		}

		public void Build(bool ForceToRebuild, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture, params string[] AdditionalPreprocessors)
		{
			if (!BuildInternal(ForceToRebuild, AdditionalPreprocessors))
				return;

			LoadModules(Configuration, Architecture);

			CheckCircularDependencies(Configuration, Architecture);

			LoadTargets(Configuration, Architecture);
		}

		public ModuleRules[] GetModuleRules(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return moduleRules.Get(Configuration, Architecture);
		}

		public ModuleRules GetModuleRules(string Name, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ModuleRules module = Array.Find(moduleRules.Get(Configuration, Architecture), (ModuleRules item) => item.Name == Name);
			if (module != null)
				return module;

			throw new FrontendException($"Couldn't find {Name} module rules");
		}

		public TargetRules[] GetTargetRules(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return targetRules.Get(Configuration, Architecture);
		}

		public TargetRules GetTargetRules(string Name, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			TargetRules target = Array.Find(targetRules.Get(Configuration, Architecture), (TargetRules item) => item.ModuleName == Name);
			if (target != null)
				return target;

			throw new FrontendException($"Couldn't find {Name} target rules");
		}

		private bool BuildInternal(bool ForceToRebuild, params string[] AdditionalPreprocessors)
		{
			bool built = builder.Built && !ForceToRebuild;

			builder.Build(ForceToRebuild, AdditionalPreprocessors);

			if (built)
				return false;

			moduleRules.Clear();
			targetRules.Clear();

			return true;
		}

		private void LoadModules(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ConsoleHelper.WriteInfo($"Loading Modules for {Configuration} {Architecture}");

			BaseRules.OperatingSystems operatingSystem = GetOperatingSystem();
			BaseRules.Configurations configuration = GetConfiguration(Configuration);
			BaseRules.Platforms platform = GetPlatform(Architecture);

			List<int> nameHashes = new List<int>();
			foreach (Type moduleType in builder.ModuleTupes)
			{
				ModuleRules module = (ModuleRules)Activator.CreateInstance(moduleType, operatingSystem, configuration, platform);

				if (nameHashes.Find((int item) => item == module.Name.GetHashCode()) != 0)
					throw new FrontendException($"A module with same name of {module.Name} already exists");

				nameHashes.Add(module.Name.GetHashCode());
				moduleRules.Add(Configuration, Architecture, module);
			}
		}

		private void CheckCircularDependencies(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ConsoleHelper.WriteInfo($"Check for Circular Dependencies {Configuration} {Architecture}");

			ModuleRules[] rules = moduleRules.Get(Configuration, Architecture);

			foreach (ModuleRules builder in rules)
			{
				Stack<string> stack = new Stack<string>();
				CheckCircularDependencies(builder, Configuration, Architecture, stack);
			}
		}

		private void LoadTargets(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ConsoleHelper.WriteInfo($"Loading Targets for {Configuration} {Architecture}");

			BaseRules.OperatingSystems operatingSystem = GetOperatingSystem();
			BaseRules.Configurations configuration = GetConfiguration(Configuration);
			BaseRules.Platforms platform = GetPlatform(Architecture);

			List<int> nameHashes = new List<int>();
			foreach (Type targetType in builder.TargetTypes)
			{
				TargetRules target = (TargetRules)Activator.CreateInstance(targetType, operatingSystem, configuration, platform);

				if (nameHashes.Find((int item) => item == target.ModuleName.GetHashCode()) != 0)
					throw new FrontendException($"A target with same name of {target.ModuleName} already exists");

				nameHashes.Add(target.ModuleName.GetHashCode());

				targetRules.Add(Configuration, Architecture, (TargetRules)Activator.CreateInstance(targetType, operatingSystem, configuration, platform));
			}
		}

		private void CheckCircularDependencies(ModuleRules Module, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture, Stack<string> ModuleNameStack)
		{
			ModuleNameStack.Push(Module.Name);

			string[] dependencies = Module.GetAllDependencies();
			foreach (string dependency in dependencies)
			{
				if (ModuleNameStack.Contains(dependency))
					throw new FrontendException($"A circular dependency between {Module.Name} and {dependency} has detected");

				CheckCircularDependencies(GetModuleRules(dependency, Configuration, Architecture), Configuration, Architecture, ModuleNameStack);
			}

			ModuleNameStack.Pop();
		}

		private static BaseRules.OperatingSystems GetOperatingSystem()
		{
			switch (EnvironmentHelper.OperatingSystem)
			{
				case EnvironmentHelper.OperatingSystems.Windows:
					return BaseRules.OperatingSystems.Windows;

				case EnvironmentHelper.OperatingSystems.Linux:
					return BaseRules.OperatingSystems.Linux;

				default:
					throw new NotImplementedException($"Handler for {EnvironmentHelper.OperatingSystem} has not implemented");
			}
		}

		private static BaseRules.Platforms GetPlatform(ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			switch (Architecture)
			{
				case ProjectBase.ProfileBase.PlatformArchitectures.x86:
					return BaseRules.Platforms.x86;

				case ProjectBase.ProfileBase.PlatformArchitectures.x64:
					return BaseRules.Platforms.x64;

				default:
					throw new NotImplementedException($"Handler for {Architecture} has not implemented");
			}
		}

		private static BaseRules.Configurations GetConfiguration(ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration)
		{
			switch (BuildConfiguration)
			{
				case ProjectBase.ProfileBase.BuildConfigurations.Debug:
					return BaseRules.Configurations.Debug;

				case ProjectBase.ProfileBase.BuildConfigurations.Release:
					return BaseRules.Configurations.Release;

				default:
					throw new NotImplementedException($"Handler for {BuildConfiguration} has not implemented");
			}
		}
	}
}