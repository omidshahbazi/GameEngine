// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ASTCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ASTCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "RenderCommon", "DataUtility", "Debugging" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "ProgramParser", "ProgramCompilerCommon", "MemoryManagement", "APIIntrinsic" }; }
			}
		}
	}
}