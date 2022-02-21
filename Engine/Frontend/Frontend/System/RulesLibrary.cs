// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using Engine.Frontend.Utilities;
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

		public void Build(bool ForceToRebuild)
		{
			builder.Build(ForceToRebuild);

			moduleRules.Clear();
			targetRules.Clear();

			TargetRules.OperatingSystems operatingSystem;
			switch (EnvironmentHelper.OperatingSystem)
			{
				case EnvironmentHelper.OperatingSystems.Windows:
					operatingSystem = TargetRules.OperatingSystems.Windows;
					break;
				case EnvironmentHelper.OperatingSystems.Linux:
					operatingSystem = TargetRules.OperatingSystems.Linux;
					break;
				default:
					throw new NotImplementedException($"Handler for {EnvironmentHelper.OperatingSystem} has not implemented");
			}

			foreach (ProjectBase.ProfileBase.BuildConfigurations buildConfiguration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures architecture in BuildSystemHelper.PlatformTypes)
				{
					BaseRules.Configurations configuration;
					switch (buildConfiguration)
					{
						case ProjectBase.ProfileBase.BuildConfigurations.Debug:
							configuration = BaseRules.Configurations.Debug;
							break;
						case ProjectBase.ProfileBase.BuildConfigurations.Release:
							configuration = BaseRules.Configurations.Release;
							break;
						default:
							throw new NotImplementedException($"Handler for {buildConfiguration} has not implemented");
					}

					BaseRules.Platforms platform;
					switch (architecture)
					{
						case ProjectBase.ProfileBase.PlatformArchitectures.x86:
							platform = BaseRules.Platforms.x86;
							break;
						case ProjectBase.ProfileBase.PlatformArchitectures.x64:
							platform = BaseRules.Platforms.x64;
							break;
						default:
							throw new NotImplementedException($"Handler for {architecture} has not implemented");
					}

					List<int> nameHashes = new List<int>();
					foreach (Type moduleType in builder.ModuleTupes)
					{
						ModuleRules module = (ModuleRules)Activator.CreateInstance(moduleType, configuration, platform);

						if (nameHashes.Find((int item) => item == module.Name.GetHashCode()) != 0)
							throw new FrontendException($"A module with same name of {module.Name} already exists");

						nameHashes.Add(module.Name.GetHashCode());

						moduleRules.Add(buildConfiguration, architecture, module);
					}

					CheckCircularDependencies(buildConfiguration, architecture);

					nameHashes.Clear();
					foreach (Type targetType in builder.TargetTypes)
					{
						TargetRules target = (TargetRules)Activator.CreateInstance(targetType, operatingSystem, configuration, platform);

						if (nameHashes.Find((int item) => item == target.ModuleName.GetHashCode()) != 0)
							throw new FrontendException($"A target with same name of {target.ModuleName} already exists");

						nameHashes.Add(target.ModuleName.GetHashCode());

						targetRules.Add(buildConfiguration, architecture, (TargetRules)Activator.CreateInstance(targetType, operatingSystem, configuration, platform));
					}
				}
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

		private void CheckCircularDependencies(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			ModuleRules[] rules = moduleRules.Get(Configuration, Architecture);

			foreach (ModuleRules builder in rules)
			{
				Stack<string> stack = new Stack<string>();
				CheckCircularDependencies(builder, Configuration, Architecture, stack);
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

			//foreach (string dependency in Module.PrivateDependencyModuleNames)
			//{
			//	if (ModuleNameStack.Contains(dependency))
			//		throw new FrontendException($"A circular dependency between {Module.Name} and {dependency} has detected");

			//	CheckCircularDependencies(GetModuleRules(dependency, Configuration, Architecture), Configuration, Architecture, ModuleNameStack);
			//}

			//foreach (string dependency in Module.PublicDependencyModuleNames)
			//{
			//	if (ModuleNameStack.Contains(dependency))
			//		throw new FrontendException($"A circular dependency between {Module.Name} and {dependency} has detected");

			//	CheckCircularDependencies(GetModuleRules(dependency, Configuration, Architecture), Configuration, Architecture, ModuleNameStack);
			//}

			ModuleNameStack.Pop();
		}
	}
}