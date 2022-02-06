// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System;
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
			return $"{a.Minutes.ToString("D2")}:{a.Seconds.ToString("D2")}";
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

		public static string GetOutputFileExtension(this ModuleRules Build)
		{
			switch (Build.UseType)
			{
				case ModuleRules.UseTypes.Executable:
					return EnvironmentHelper.ExecutableExtentions;

				case ModuleRules.UseTypes.DynamicLibrary:
					return EnvironmentHelper.DynamicLibraryExtentions;

				case ModuleRules.UseTypes.StaticLibrary:
					return EnvironmentHelper.StaticLibraryExtentions;

				default:
					throw new NotImplementedException($"Handler for {Build.UseType} has not implemented");
			}
		}

		private static string[] CSharpFiles = null;
		public static string GetSourceRootDirectory(this ModuleRules Self)
		{
			if (CSharpFiles == null)
				CSharpFiles = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CSharpFileExtensions);

			string fileName = Self.Name + ModuleRules.FilePostfix;

			string fullPath = Array.Find(CSharpFiles, (string item) => item.EndsWith(EnvironmentHelper.PathSeparator + fileName));

			if (string.IsNullOrEmpty(fullPath))
				throw new FrontendException($"Couldn't find {fileName} module rules file");

			return Path.GetDirectoryName(fullPath) + EnvironmentHelper.PathSeparator;
		}

		public static string[] GetAllDependencies(this ModuleRules Self)
		{
			List<string> dependencies = new List<string>();

			dependencies.AddRange(Self.PrivateDependencyModuleNames);
			dependencies.AddRange(Self.PublicDependencyModuleNames);

			return dependencies.ToArray();
		}

		public static string GetNamePreprocessor(this ModuleRules Module)
		{
			return "MODULE_NAME=\"" + (Module == null ? "" : Module.Name) + "\"";
		}

		public static string GetAPIPreprocessor(this ModuleRules Module, BuildSystemHelper.APIPreprocessorTypes Value)
		{
			return Module.Name.GetAPIPreprocessorName().GetAPIPreprocessorRaw(Value);
		}

		public static string GetExternPreprocessor(this ModuleRules Module, BuildSystemHelper.ExternPreprocessorTypes Value)
		{
			return Module.Name.ToUpper() + "_EXTERN=" + (Value == BuildSystemHelper.ExternPreprocessorTypes.Empty ? "" : "extern");
		}

		public static string GetIntermediateDirectory(this string ModuleName)
		{
			return EnvironmentHelper.IntermediateDirectory + ModuleName + EnvironmentHelper.PathSeparator;
		}

		public static string GetIntermediateGeneratedDirectory(this string ModuleName)
		{
			return ModuleName.GetIntermediateDirectory() + EnvironmentHelper.GeneratedPathName + EnvironmentHelper.PathSeparator;
		}

		public static string GetIntermediateTempPath(this string ModuleName, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return ModuleName.GetIntermediateDirectory() + EnvironmentHelper.GetOutputPathName(Configuration, Architecture);
		}

		public static string GetIntermediateOutputPath(this string ModuleName, ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return ModuleName.GetIntermediateTempPath(Configuration, Architecture) + EnvironmentHelper.BinariesPathName + EnvironmentHelper.PathSeparator;
		}

		public static ProjectBase.ProfileBase.OutputTypes ToOutputType(this ModuleRules.UseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case ModuleRules.UseTypes.Executable:
					return ProjectBase.ProfileBase.OutputTypes.Application;

				case ModuleRules.UseTypes.DynamicLibrary:
					return ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;

				case ModuleRules.UseTypes.StaticLibrary:
					return ProjectBase.ProfileBase.OutputTypes.StaticLinkLibrary;

				default:
					throw new NotImplementedException($"Handler for {LibraryUseType} has not implemented");
			}
		}

		public static string GetPreprocessor(this EnvironmentHelper.OperatingSystems OperatingSystem)
		{
			return OperatingSystem.ToString().ToUpper();
		}

		public static string GetPrettyName(this ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			string type = "";

			switch (EnvironmentHelper.OperatingSystem)
			{
				case EnvironmentHelper.OperatingSystems.Windows:
					type += "Win";
					break;
				case EnvironmentHelper.OperatingSystems.Linux:
					type += "Linux";
					break;

				default:
					throw new NotImplementedException($"Handler for {EnvironmentHelper.OperatingSystem} has not implemented");
			}

			switch (Architecture)
			{
				case ProjectBase.ProfileBase.PlatformArchitectures.x86:
					type += "32";
					break;

				case ProjectBase.ProfileBase.PlatformArchitectures.x64:
					type += "64";
					break;

				default:
					throw new NotImplementedException($"Handler for {Architecture} has not implemented");
			}

			return type;
		}

		public static string GetPreprocessor(this ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return Architecture.ToString().ToUpper();
		}

		public static string GetPreprocessor(this ProjectBase.ProfileBase.BuildConfigurations Configuration)
		{
			return Configuration.ToString().ToUpper() + "_MODE";
		}

		public static string GetAPIPreprocessorName(this string Name)
		{
			return Name.ToUpper() + "_API";
		}

		public static string GetPreprocessorValue(this BuildSystemHelper.APIPreprocessorTypes Value)
		{
			return (Value == BuildSystemHelper.APIPreprocessorTypes.Empty ? "" : ("__declspec(dll" + (Value == BuildSystemHelper.APIPreprocessorTypes.Import ? "import" : "export") + ")"));
		}

		public static string GetAPIPreprocessorRaw(this string Name, BuildSystemHelper.APIPreprocessorTypes Value)
		{
			return Name + "=" + Value.GetPreprocessorValue();
		}
	}
}
