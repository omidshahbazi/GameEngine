// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.System.Build;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;

namespace Engine.Frontend.Utilities
{
	static class Extensions
	{
		public static string ToHHMMSS(this TimeSpan a)
		{
			return a.ToString(@"hh\:mm\:ss");
		}

		public static void AppendIndent(this StringBuilder Builder, uint Indent = 0)
		{
			for (uint i = 0; i < Indent; ++i)
				Builder.Append('\t');
		}

		public static Type[] GetTypes<T>(this Assembly Self) where T : class
		{
			List<Type> result = new List<Type>();

			Type[] types = Self.GetTypes();

			Type baseType = typeof(T);

			foreach (Type type in types)
			{
				if (!baseType.IsAssignableFrom(type))
					continue;

				result.Add(type);
			}

			return result.ToArray();
		}

		public static string GetSourceRootDirectory(this ModuleRules Self)
		{
			string fileName = Self.Name + ModuleRules.FilePostfix;

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, fileName);
			if (files.Length == 0)
			{
				ConsoleHelper.WriteError("Couldn't find the {0}", fileName);

				return string.Empty;
			}

			return Path.GetDirectoryName(files[0]);
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
