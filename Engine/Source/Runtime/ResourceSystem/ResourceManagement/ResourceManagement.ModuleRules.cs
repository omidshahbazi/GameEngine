// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
	class ResourceManagementModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ResourceManagement"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public ResourceManagementModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("OBJAssetParser");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("Containers");

			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("ResourceSystem");
			PublicDependencyModuleNames.Add("ResourceCommon");
		}
	}
}