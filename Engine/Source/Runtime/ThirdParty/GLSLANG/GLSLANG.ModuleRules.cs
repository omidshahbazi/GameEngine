// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	// Update notes
	// 1. Pull from https://github.com/KhronosGroup/glslang
	// 2. Remove extra item checks in CMake
	// 3. Build
	// 4. Update using built files

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
#if DEBUG_MODE
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Debug/Win32/glslangd.lib", "lib/Debug/Win32/OGLCompilerd.lib", "lib/Debug/Win32/OSDependentd.lib", "lib/Debug/Win32/SPIRVd.lib", "lib/Debug/Win32/MachineIndependentd.lib", "lib/Debug/Win32/GenericCodeGend.lib" }; }
			}
#elif RELEASE_MODE
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/Win32/glslang.lib", "lib/Release/Win32/OGLCompiler.lib", "lib/Release/Win32/OSDependent.lib", "lib/Release/Win32/SPIRV.lib", "lib/Debug/Win32/MachineIndependent.lib", "lib/Release/Win32/GenericCodeGen.lib" }; }
			}
#endif
#elif X64
#if DEBUG_MODE
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Debug/x64/glslangd.lib", "lib/Debug/x64/OGLCompilerd.lib", "lib/Debug/x64/OSDependentd.lib", "lib/Debug/x64/SPIRVd.lib", "lib/Debug/x64/MachineIndependentd.lib", "lib/Debug/x64/GenericCodeGend.lib" }; }
			}
#elif RELEASE_MODE
			public override string[] LibraryPaths
			{
				get { return new string[] { "lib/Release/x64/glslang.lib", "lib/Release/x64/OGLCompiler.lib", "lib/Release/x64/OSDependent.lib", "lib/Release/x64/SPIRV.lib", "lib/Release/x64/MachineIndependent.lib", "lib/Release/x64/GenericCodeGen.lib" }; }
			}
#endif
#endif
		}
	}
}