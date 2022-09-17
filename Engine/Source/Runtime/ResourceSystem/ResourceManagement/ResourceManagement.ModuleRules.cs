// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
	class ResourceManagementModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ResourceManagement"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ResourceManagementModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("BuiltInResources");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("Debugging");

			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("ResourceSystem");
			PublicDependencyModuleNames.Add("ResourceCommon");
		}
	}
}