// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	class GLEWModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLEW"; }
		}

		public class BuildRules : BuildRulesBase
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
#if WIN32
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/Win32/glew32.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/Release/Win32/glew32.dll" }; }
			}
#elif X64
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/x64/glew32.lib" }; }
			}

			public override string[] BinaryPaths
			{
				get { return new string[] { "bin/Release/x64/glew32.dll" }; }
			}
#endif
		}
	}
}