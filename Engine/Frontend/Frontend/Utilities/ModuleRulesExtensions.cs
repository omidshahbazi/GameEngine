// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System;
using System;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.Utilities
{
	static class ModuleRulesExtensions
	{
		private static string[] files = null;

		public static string GetSourceRootDirectory(this ModuleRules Self)
		{
			if (files == null)
				files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CSharpFileExtensions);

			string fileName = Self.Name + ModuleRules.FilePostfix;

			string fullPath = Array.Find(files, (string item) => item.EndsWith(EnvironmentHelper.PathSeparator + fileName));

			if (string.IsNullOrEmpty(fullPath))
				throw new FrontendException($"Couldn't find {fileName} module rules file");

			return Path.GetDirectoryName(fullPath) + EnvironmentHelper.PathSeparator;
		}

		public static ModuleRules GetModule(this TargetRules Self, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Platform)
		{
			ModuleRules[] modules = RulesLibrary.Instance.GetModuleRules(Configuration, Platform);

			ModuleRules module = Array.Find(modules, (ModuleRules item) => item.Name == Self.ModuleName);
			if (module != null)
				return module;

			throw new FrontendException($"Couldn't find {Self.ModuleName} module rules");
		}

		public static string[] GetAllDependencies(this ModuleRules Self)
		{
			List<string> dependencies = new List<string>();

			dependencies.AddRange(Self.PrivateDependencyModuleNames);
			dependencies.AddRange(Self.PublicDependencyModuleNames);

			return dependencies.ToArray();
		}
	}
}
