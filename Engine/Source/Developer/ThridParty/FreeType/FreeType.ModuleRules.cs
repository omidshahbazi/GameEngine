// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ThirdParty
{
	class FreeTypeModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FreeType"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.UseOnly; }
		}

		public FreeTypeModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			IncludePaths.Add("include/");

			if (Platform == Platforms.x86)
			{
				LibraryPaths.Add("lib/win32/freetype.lib");
				BinaryPaths.Add("bin/win32/freetype.dll");
			}
			else
			{
				LibraryPaths.Add("lib/win64/freetype.lib");
				BinaryPaths.Add("bin/win64/freetype.dll");
			}
		}
	}
}