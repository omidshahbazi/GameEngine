// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
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
					throw new Exception(LibraryUseType + " cannot cast to OutputTypes");
			}
		}

		public static string ReflectionModuleName
		{
			get { return "Reflection"; }
		}

		public static string GeneratedPathName
		{
			get { return "Generated"; }
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
			return GetAPIPreprocessorName(Name) + "=" + (Value == APIPreprocessorTypes.Empty ? "" : ("__declspec(dll" + (Value == APIPreprocessorTypes.Import ? "import" : "export") + ")"));
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
			}

			throw new NotImplementedException(Platform.ToString() + " not implemented");
		}

		public static string GetPlatformTypesPreprocessor(ProjectBase.ProfileBase.PlatformArchitectures PlatformType)
		{
			return PlatformType.ToString().ToUpper();
		}
	}
}
