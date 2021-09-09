// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ProgramCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ProgramCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "ProgramParser", "ASTCompiler", "DynamicModuleSystem", "Debugging", "ProgramCompilerCommon" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "RenderCommon", "RenderDevice", "MemoryManagement" }; }
			}
		}
	}
}