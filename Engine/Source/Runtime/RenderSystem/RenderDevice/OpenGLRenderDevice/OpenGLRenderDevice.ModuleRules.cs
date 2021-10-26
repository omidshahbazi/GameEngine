// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class OpenGLRenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "OpenGLRenderDevice"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public OpenGLRenderDeviceModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("RenderDevice");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("WindowUtility");
			PrivateDependencyModuleNames.Add("GLEW");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");

			DependencyStaticLibraries.Add("opengl32.lib");
		}
	}
}