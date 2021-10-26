// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class RenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderDevice"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public RenderDeviceModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("MathContainers");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("WindowUtility");
			PublicDependencyModuleNames.Add("RenderCommon");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
		}
	}
}