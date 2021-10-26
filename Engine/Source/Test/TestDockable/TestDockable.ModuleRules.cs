// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
	class TestDockableModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TestDockable"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.Executable; }
		}

		public TestDockableModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("EditorGUI");
			PrivateDependencyModuleNames.Add("ResourceManagement");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("JSON");
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");

			GenerateRenderDocSettings = true;
		}
	}
}