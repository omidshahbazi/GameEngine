// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.ThirdParty
{
	class FreeTypeModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FreeType"; }
		}

		public class BuildRulesX86 : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "FreeType"; }
			}

			public override Platforms Platform
			{
				get { return Platforms.x86; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
			}

			public override string[] IncludePaths
			{
				get { return new string[] { "include/" }; }
			}

			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/win32/freetype.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/win32/freetype.dll" }; }
			}
		}

		public class BuildRulesX64 : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "FreeType"; }
			}

			public override Platforms Platform
			{
				get { return Platforms.x64; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
			}

			public override string[] IncludePaths
			{
				get { return new string[] { "include/" }; }
			}

			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/win64/freetype.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/win64/freetype.dll" }; }
			}
		}
	}
}