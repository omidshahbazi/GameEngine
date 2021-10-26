// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	class GLEWModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLEW"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.UseOnly; }
		}

		public GLEWModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			if (Platform == Platforms.x86)
			{
				LibraryPaths.Add("lib/Release/Win32/glew32.lib");
				BinaryPaths.Add("bin/Release/Win32/glew32.dll");
			}
			else
			{
				LibraryPaths.Add("lib/Release/x64/glew32.lib");
				BinaryPaths.Add("bin/Release/x64/glew32.dll");
			}
		}
	}
}