// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using System;
using System.Collections.Generic;

namespace Engine.Frontend.System
{
	class RulesLibrary
	{
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

		private ConfigurationMap moduleRules = null;
		private List<TargetRules> targetRules = null;

		public TargetRules[] TargetRules
		{
			get { return targetRules.ToArray(); }
		}

		public static RulesLibrary Instance
		{
			get { return instance; }
		}

		private RulesLibrary()
		{
			builder = new RulesLibraryBuilder();

			moduleRules = new ConfigurationMap();
			targetRules = new List<TargetRules>();
		}

		public void Build(bool ForceToRebuild)
		{
			builder.Build(ForceToRebuild);

			foreach (ProjectBase.ProfileBase.BuildConfigurations buildConfiguration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures platformType in BuildSystemHelper.PlatformTypes)
				{
					ModuleRules.Configurations configuration;
					switch (buildConfiguration)
					{
						case ProjectBase.ProfileBase.BuildConfigurations.Debug:
							configuration = ModuleRules.Configurations.Debug;
							break;
						case ProjectBase.ProfileBase.BuildConfigurations.Release:
							configuration = ModuleRules.Configurations.Release;
							break;
						default:
							throw new NotImplementedException($"Handler for {BuildSystemHelper.BuildConfiguration} has not implemented");
					}

					ModuleRules.Platforms platform;
					switch (platformType)
					{
						case ProjectBase.ProfileBase.PlatformArchitectures.x86:
							platform = ModuleRules.Platforms.x86;
							break;
						case ProjectBase.ProfileBase.PlatformArchitectures.x64:
							platform = ModuleRules.Platforms.x64;
							break;
						default:
							throw new NotImplementedException($"Handler for {BuildSystemHelper.BuildConfiguration} has not implemented");
					}

					PlatformMap platformMap = null;
					if (moduleRules.ContainsKey(buildConfiguration))
						platformMap = moduleRules[buildConfiguration];
					else
						platformMap = moduleRules[buildConfiguration] = new PlatformMap();

					ModuleRulesList list = null;
					if (platformMap.ContainsKey(platformType))
						list = platformMap[platformType];
					else
						list = platformMap[platformType] = new ModuleRulesList();

					foreach (Type moduleType in builder.ModuleTupes)
						list.Add((ModuleRules)Activator.CreateInstance(moduleType, configuration, platform));
				}

			foreach (Type targetType in builder.TargetTypes)
				targetRules.Add((TargetRules)Activator.CreateInstance(targetType));
		}

		public ModuleRules[] GetModuleRules(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Platform)
		{
			if (!moduleRules.ContainsKey(Configuration))
				throw new NotImplementedException($"Handler for {Configuration} has not implemented");

			PlatformMap platformMap = moduleRules[Configuration];

			if (!platformMap.ContainsKey(Platform))
				throw new NotImplementedException($"Handler for {Platform} has not implemented");

			return platformMap[Platform].ToArray();
		}
	}
}