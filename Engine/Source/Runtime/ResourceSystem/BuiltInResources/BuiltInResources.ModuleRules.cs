// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
	class BuiltInResourcesModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "BuiltInResources"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public BuiltInResourcesModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("FileUtility");

			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("ResourceSystem");
			PublicDependencyModuleNames.Add("ResourceCommon");
		}
	}
}