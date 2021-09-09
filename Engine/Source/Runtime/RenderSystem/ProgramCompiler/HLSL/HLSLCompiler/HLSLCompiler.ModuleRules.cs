// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class HLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "HLSLCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "HLSLCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Platform", "ProgramCompilerCommon", "RenderCommon", "DynamicModuleDefinition" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "d3dcompiler.lib" }; }
			}
		}
	}
}