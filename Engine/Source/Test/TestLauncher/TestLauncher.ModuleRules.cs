// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
    class TestLauncherModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestLauncher"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.Executable; }
		}

		public TestLauncherModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("CoreSystem");
			PrivateDependencyModuleNames.Add("GameObjectSystem");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("ResourceManagement");
			PrivateDependencyModuleNames.Add("ResourceCommon");
			PrivateDependencyModuleNames.Add("WindowUtility");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("InputSystem");
			PrivateDependencyModuleNames.Add("Mathematics");

			GenerateRenderDocSettings = true;

			IconPath = "Engine/Contents/Icon.png";
		}
    }
}