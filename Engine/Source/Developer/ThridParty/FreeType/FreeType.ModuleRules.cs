// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ThirdParty
{
	class FreeTypeModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FreeType"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.UseOnly; }
		}

		public FreeTypeModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			IncludePaths.Add("include/");

			if (Platform == Platforms.x86)
			{
				LibraryPaths.Add("lib/Win32/freetype.lib");
				BinaryPaths.Add("bin/Win32/freetype.dll");
			}
			else
			{
				LibraryPaths.Add("lib/Win64/freetype.lib");
				BinaryPaths.Add("bin/Win64/freetype.dll");
			}
		}
	}
}