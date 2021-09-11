// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class OpenGLRenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "OpenGLRenderDevice"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "OpenGLRenderDevice"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "Platform", "RenderDevice", "RenderCommon", "Debugging", "WindowUtility", "GLEW", "DynamicModuleDefinition", "ProgramCompilerCommon" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "opengl32.lib" }; }
			}
		}
	}
}