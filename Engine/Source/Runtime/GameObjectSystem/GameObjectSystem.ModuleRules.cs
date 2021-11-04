// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
    class GameObjectSystemModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "GameObjectSystem"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public GameObjectSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("MathContainers");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("ResourceManagement");
			PrivateDependencyModuleNames.Add("Profiler");
		}
    }
}