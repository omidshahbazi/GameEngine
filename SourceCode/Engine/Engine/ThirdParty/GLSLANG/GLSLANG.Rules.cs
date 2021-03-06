// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	// Update notes
	// 1. Pull from https://github.com/KhronosGroup/glslang
	// 2. Build it using instructions in the landing page
	// 3. Update using built files

	class GLSLANGRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "GLSLANG"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "GLSLANG"; }
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
				get { return new string[] { "lib/glslangd.lib", "lib/OGLCompilerd.lib", "lib/OSDependentd.lib", "lib/SPIRVd.lib", "lib/MachineIndependentd.lib", "lib/GenericCodeGend.lib" }; }
			}
		}
	}
}