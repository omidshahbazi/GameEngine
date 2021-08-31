// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLEWModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLEW"; }
		}

		public class BuildRulesX86 : BuildRulesBase
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

			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/Win32/glew32.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/Release/Win32/glew32.dll" }; }
			}
		}

		public class BuildRulesX64 : BuildRulesBase
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

			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/x64/glew32.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/Release/x64/glew32.dll" }; }
			}
		}
	}
}