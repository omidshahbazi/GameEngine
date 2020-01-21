// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class FreeTypeRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "FreeType"; }
		}

		public class RuleX86 : RuleBase
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

			public override string[] IncludesPath
			{
				get { return new string[] { "include/" }; }
			}

			public override string[] LibrariesPath
			{
				get { return new string[] { "lib/win32/freetype.lib" }; }
			}

			public override string[] BinariesPath
			{
				get { return new string[] { "bin/win32/freetype.dll" }; }
			}
		}

		public class RuleX64 : RuleBase
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

			public override string[] IncludesPath
			{
				get { return new string[] { "include/" }; }
			}

			public override string[] LibrariesPath
			{
				get { return new string[] { "lib/win64/freetype.lib" }; }
			}

			public override string[] BinariesPath
			{
				get { return new string[] { "bin/win64/freetype.dll" }; }
			}
		}
	}
}