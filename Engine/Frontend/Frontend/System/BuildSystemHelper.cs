// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;
using System;

namespace Engine.Frontend.System
{
	static class BuildSystemHelper
	{
		public static readonly ProjectBase.ProfileBase.BuildConfigurations[] BuildConfigurations = { ProjectBase.ProfileBase.BuildConfigurations.Debug, ProjectBase.ProfileBase.BuildConfigurations.Release };
		public static readonly ProjectBase.ProfileBase.PlatformArchitectures[] PlatformTypes = { ProjectBase.ProfileBase.PlatformArchitectures.x86, ProjectBase.ProfileBase.PlatformArchitectures.x64 };

		public enum APIPreprocessorTypes
		{
			Empty,
			Import,
			Export
		}

		public enum ExternPreprocessorTypes
		{
			Empty,
			Fill
		}

		public static ProjectBase.ProfileBase.PlatformArchitectures PlatformArchitecture
		{
			get;
			set;
		}

		public static ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration
		{
			get;
			set;
		}

		public static ProjectBase.ProfileBase.OutputTypes LibraryUseTypesToOutputType(ModuleRules.LibraryUseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case ModuleRules.LibraryUseTypes.Executable:
					return ProjectBase.ProfileBase.OutputTypes.Application;

				case ModuleRules.LibraryUseTypes.DynamicLibrary:
					return ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;

				case ModuleRules.LibraryUseTypes.StaticLibrary:
					return ProjectBase.ProfileBase.OutputTypes.StaticLinkLibrary;

				default:
					throw new NotImplementedException($"Handler for {LibraryUseType} has not implemented");
			}
		}

		public static string GetConfigurationModePreprocessor(ProjectBase.ProfileBase.BuildConfigurations Configuration)
		{
			return Configuration.ToString().ToUpper() + "_MODE";
		}

		public static string GetAPIPreprocessorName(string Name)
		{
			return Name.ToUpper() + "_API";
		}

		public static string GetAPIPreprocessor(string Name, APIPreprocessorTypes Value)
		{
			return GetAPIPreprocessorRaw(GetAPIPreprocessorName(Name), Value);
		}

		public static string GetExportAPIPreprocessorRaw()
		{
			return GetAPIPreprocessorRaw("EXPORT_API", BuildSystemHelper.APIPreprocessorTypes.Export);
		}

		public static string GetAPIPreprocessorRaw(string Name, APIPreprocessorTypes Value)
		{
			return Name + "=" + GetAPIPreprocessorValue(Value);
		}

		public static string GetAPIPreprocessorValue(APIPreprocessorTypes Value)
		{
			return (Value == APIPreprocessorTypes.Empty ? "" : ("__declspec(dll" + (Value == APIPreprocessorTypes.Import ? "import" : "export") + ")"));
		}

		public static string GetExternPreprocessor(string Name, ExternPreprocessorTypes Value)
		{
			return Name.ToUpper() + "_EXTERN=" + (Value == ExternPreprocessorTypes.Empty ? "" : "extern");
		}

		public static string GetModuleNamePreprocessor(string Name)
		{
			return "MODULE_NAME=\"" + Name + "\"";
		}

		public static string GetPlatformPreprocessor(EnvironmentHelper.Platforms Platform)
		{
			return Platform.ToString().ToUpper();
		}

		public static string GetPlatformType(CPPProject.Profile.PlatformArchitectures Platform)
		{
			switch (Platform)
			{
				case CPPProject.Profile.PlatformArchitectures.x86:
					return "Win32";

				case CPPProject.Profile.PlatformArchitectures.x64:
					return "x64";

				default:
					throw new NotImplementedException($"Handler for {Platform} has not implemented");
			}
		}

		public static string GetPlatformTypesPreprocessor(ProjectBase.ProfileBase.PlatformArchitectures PlatformType)
		{
			return PlatformType.ToString().ToUpper();
		}

		public static string GetOutputPathName(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return $"{Configuration} {Architecture}" + EnvironmentHelper.PathSeparator;
		}

		public static string GetOutputDirectory(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return EnvironmentHelper.BinariesDirectory + GetOutputPathName(Configuration, Architecture);
		}

		public static string GetExtension(ModuleRules.BuildRulesBase Build)
		{
			switch (Build.LibraryUseType)
			{
				case ModuleRules.LibraryUseTypes.Executable:
					return EnvironmentHelper.ExecutableExtentions;

				case ModuleRules.LibraryUseTypes.DynamicLibrary:
					return EnvironmentHelper.DynamicLibraryExtentions;

				case ModuleRules.LibraryUseTypes.StaticLibrary:
					return EnvironmentHelper.StaticLibraryExtentions;

				default:
					throw new NotImplementedException($"Handler for {Build.LibraryUseType} has not implemented");
			}
		}
	}
}
