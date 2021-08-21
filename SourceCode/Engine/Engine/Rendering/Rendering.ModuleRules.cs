// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class RenderingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Rendering"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Rendering"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Containers", "MathContainers", "MemoryManagement", "DataUtility", "TimeUtility", "Threading", "Lexer", "GLEW", "GLSLANG", "ResourceCommon" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "WindowUtility" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "opengl32.lib", "d3d12.lib", "dxgi.lib", "d3dcompiler.lib" }; }
			}

			public override string[] PreprocessorDefinitions
			{
				get { return new string[] { "GLM_ENABLE_EXPERIMENTAL" }; }
			}

			public override bool GenerateReflection
			{
				get { return false; }
			}
		}
	}
}