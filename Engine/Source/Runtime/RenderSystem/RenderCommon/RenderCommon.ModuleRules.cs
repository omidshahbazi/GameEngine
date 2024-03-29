// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem
{
	class RenderCommonModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderCommon"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public RenderCommonModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Allocators");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("Platform");
			PublicDependencyModuleNames.Add("MemoryManagement");

			PrivateDependencyModuleNames.Add("DataUtility");
		}
	}
}