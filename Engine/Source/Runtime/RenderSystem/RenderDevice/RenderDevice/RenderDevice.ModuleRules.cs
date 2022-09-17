// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class RenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderDevice"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public RenderDeviceModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("MathContainers");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("WindowUtility");
			PublicDependencyModuleNames.Add("RenderCommon");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
		}
	}
}