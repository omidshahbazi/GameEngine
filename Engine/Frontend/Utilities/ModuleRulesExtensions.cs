// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.System.Build;
using GameFramework.Common.Utilities;
using System;
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

			string fullPath = Array.Find(files, (string item) => item.EndsWith(fileName));
			if (string.IsNullOrEmpty(fullPath))
			{
				ConsoleHelper.WriteError("Couldn't find the {0}", fileName);

				return string.Empty;
			}

			return Path.GetDirectoryName(fullPath);
		}

		public static ModuleRules GetModule(this TargetRules Self)
		{
			foreach (ModuleRules module in RuleLibraryBuilder.Instance.Modules)
			{
				if (module.Name != Self.ModuleName)
					continue;

				return module;
			}

			return null;
		}
	}
}
