// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.System.Build;
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

		public static ModuleRules GetModule(this TargetRules Self)
		{
			foreach (ModuleRules module in RuleLibraryBuilder.Instance.Modules)
			{
				if (module.Name != Self.ModuleName)
					continue;

				return module;
			}

			throw new FrontendException($"Couldn't find {Self.ModuleName} module rules");
		}

		public static string[] GetAllDependencies(this ModuleRules.BuildRulesBase Self)
		{
			List<string> dependencies = new List<string>();

			if (Self.PrivateDependencyModuleNames != null)
				dependencies.AddRange(Self.PrivateDependencyModuleNames);

			if (Self.PublicDependencyModuleNames != null)
				dependencies.AddRange(Self.PublicDependencyModuleNames);

			return dependencies.ToArray();
		}
	}
}
