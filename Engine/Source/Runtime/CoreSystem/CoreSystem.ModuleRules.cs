// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class CoreSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "CoreSystem"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public CoreSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("InputSystem");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("GameObjectSystem");
			PrivateDependencyModuleNames.Add("ResourceManagement");
			PrivateDependencyModuleNames.Add("Profiler");
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("WindowUtility");
			PublicDependencyModuleNames.Add("TimeUtility");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderSystem");
		}
	}
}