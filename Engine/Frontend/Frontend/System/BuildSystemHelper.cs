// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;

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

		public static readonly ProjectBase.ProfileBase.BuildConfigurations[] BuildConfigurations = { ProjectBase.ProfileBase.BuildConfigurations.Debug, ProjectBase.ProfileBase.BuildConfigurations.Release };
		public static readonly ProjectBase.ProfileBase.PlatformArchitectures[] PlatformTypes = { ProjectBase.ProfileBase.PlatformArchitectures.x86, ProjectBase.ProfileBase.PlatformArchitectures.x64 };

		public static readonly string ExportAPIPreprocessor = "EXPORT_API".GetAPIPreprocessorRaw(APIPreprocessorTypes.Export);
		public static readonly string EmptyModuleNamePreprocessor = ((ModuleRules)null).GetNamePreprocessor();
	}
}
