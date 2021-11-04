// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	// Update notes
	// 1. Download from https://github.com/nigels-com/glew/releases

	class GLEWModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLEW"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.UseOnly; }
		}

		public GLEWModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			if (Platform == Platforms.x86)
			{
				if (Configuration == Configurations.Debug)
				{
					BinaryPaths.Add("bin/Debug/Win32/glew32d.dll");
					LibraryPaths.Add("lib/Debug/Win32/glew32d.lib");
				}
				else
				{
					BinaryPaths.Add("bin/Release/Win32/glew32.dll");
					LibraryPaths.Add("lib/Release/Win32/glew32.lib");
				}
			}
			else
			{
				if (Configuration == Configurations.Debug)
				{
					BinaryPaths.Add("bin/Debug/Win64/glew32d.dll");
					LibraryPaths.Add("lib/Debug/Win64/glew32d.lib");
				}
				else
				{
					BinaryPaths.Add("bin/Release/Win64/glew32.dll");
					LibraryPaths.Add("lib/Release/Win64/glew32.lib");
				}
			}
		}
	}
}