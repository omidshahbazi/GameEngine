// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using System;

namespace Engine.Frontend.System
{
	static class BuildSystemHelper
	{
		public enum APIPreprocessorValues
		{
			Empty,
			Import,
			Export
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

		public static string GetConfigurationModePreprocessor(ProjectBase.ProfileBase.BuildConfigurations Configuration)
		{
			return Configuration.ToString().ToUpper() + "_MODE";
		}

		public static string GetAPIPreprocessor(string Name, APIPreprocessorValues Value)
		{
			return Name.ToUpper() + "_API=" + (Value == APIPreprocessorValues.Empty ? "" : ("__declspec(dll" + (Value == APIPreprocessorValues.Import ? "import" : "export") + ")"));
		}
	}
}
