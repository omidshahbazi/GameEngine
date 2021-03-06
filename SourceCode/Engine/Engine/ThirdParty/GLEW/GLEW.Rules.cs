// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLEWRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "GLEW"; }
		}

		public class RuleX86 : RuleBase
		{
			public override string TargetName
			{
				get { return "GLEW"; }
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
				get { return new string[] { "lib/Release/Win32/glew32.lib" }; }
			}

			public override string[] BinariesPath
			{
				get { return new string[] { "bin/Release/Win32/glew32.dll" }; }
			}
		}

		public class RuleX64 : RuleBase
		{
			public override string TargetName
			{
				get { return "GLEW"; }
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
				get { return new string[] { "lib/Release/x64/glew32.lib" }; }
			}

			public override string[] BinariesPath
			{
				get { return new string[] { "bin/Release/x64/glew32.dll" }; }
			}
		}
	}
}