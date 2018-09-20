// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using System;

namespace Engine.Frontend.System
{
	static class BuildSystemHelper
	{
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

		public static ProjectBase.ProfileBase.OutputTypes LibraryUseTypesToOutputType(BuildRules.LibraryUseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case BuildRules.LibraryUseTypes.Executable:
					return ProjectBase.ProfileBase.OutputTypes.Application;

				case BuildRules.LibraryUseTypes.DynamicLibrary:
					return ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;

				case BuildRules.LibraryUseTypes.StaticLibrary:
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

		public static string GetAPIPreprocessor(string Name, APIPreprocessorTypes Value)
		{
			return Name.ToUpper() + "_API=" + (Value == APIPreprocessorTypes.Empty ? "" : ("__declspec(dll" + (Value == APIPreprocessorTypes.Import ? "import" : "export") + ")"));
		}

		public static string GetExternPreprocessor(string Name, ExternPreprocessorTypes Value)
		{
			return Name.ToUpper() + "_EXTERN=" + (Value == ExternPreprocessorTypes.Empty ? "" : "extern");
		}

		public static string GetPlatformPreprocessor(EnvironmentHelper.Platforms Platform)
		{
			return Platform.ToString().ToUpper();
		}

		public static string GetPlatformTypesPreprocessor(ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
			return PlatformType.ToString().ToUpper();
		}
	}
}
