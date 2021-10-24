// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	// Update notes
	// 1. Pull from https://github.com/KhronosGroup/glslang
	// 2. Build it using instructions in the landing page
	// 3. Update using built files

	class GLSLANGModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLSLANG"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "GLSLANG"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
			}

			public override string[] IncludePaths
			{
				get { return new string[] { "include/" }; }
			}

#if WIN32
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/Win32/glslangd.lib", "lib/Release/Win32/OGLCompilerd.lib", "lib/Release/Win32/OSDependentd.lib", "lib/Release/Win32/SPIRVd.lib", "lib/Release/Win32/MachineIndependentd.lib", "lib/Release/Win32/GenericCodeGend.lib" }; }
			}
#elif X64
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/x64/glslang.lib", "lib/Release/x64/OGLCompiler.lib", "lib/Release/x64/OSDependent.lib", "lib/Release/x64/SPIRV.lib", "lib/Release/x64/MachineIndependent.lib", "lib/Release/x64/GenericCodeGen.lib" }; }
			}
#endif
		}
	}
}