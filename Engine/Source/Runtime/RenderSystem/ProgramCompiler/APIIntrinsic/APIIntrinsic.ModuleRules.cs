// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class AAPIIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "APIIntrinsic"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "APIIntrinsic"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "DynamicModuleSystem", "DataUtility" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "ProgramParser", "ProgramCompilerCommon", "Debugging" }; }
			}
		}
	}
}