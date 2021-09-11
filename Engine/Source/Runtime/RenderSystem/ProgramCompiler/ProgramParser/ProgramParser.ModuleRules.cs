// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ProgramParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ProgramParser"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Debugging" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers" , "Lexer", "Containers", "ProgramCompilerCommon", "RenderCommon", "MemoryManagement" }; }
			}
		}
	}
}