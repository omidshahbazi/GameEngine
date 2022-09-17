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

		public override UseTypes UseType
		{
			get { return UseTypes.UseOnly; }
		}

		public GLSLANGModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			IncludePaths.Add("include/");

			if (Platform == Platforms.x86)
			{
				if (Configuration == Configurations.Debug)
				{
					LibraryPaths.Add("lib/Debug/Win32/glslangd.lib");
					LibraryPaths.Add("lib/Debug/Win32/OGLCompilerd.lib");
					LibraryPaths.Add("lib/Debug/Win32/OSDependentd.lib");
					LibraryPaths.Add("lib/Debug/Win32/SPIRVd.lib");
					LibraryPaths.Add("lib/Debug/Win32/MachineIndependentd.lib");
					LibraryPaths.Add("lib/Debug/Win32/GenericCodeGend.lib");
				}
				else
				{
					LibraryPaths.Add("lib/Release/Win32/glslang.lib");
					LibraryPaths.Add("lib/Release/Win32/OGLCompiler.lib");
					LibraryPaths.Add("lib/Release/Win32/OSDependent.lib");
					LibraryPaths.Add("lib/Release/Win32/SPIRV.lib");
					LibraryPaths.Add("lib/Release/Win32/MachineIndependent.lib");
					LibraryPaths.Add("lib/Release/Win32/GenericCodeGen.lib");
				}
			}
			else
			{
				if (Configuration == Configurations.Debug)
				{
					LibraryPaths.Add("lib/Debug/Win64/glslangd.lib");
					LibraryPaths.Add("lib/Debug/Win64/OGLCompilerd.lib");
					LibraryPaths.Add("lib/Debug/Win64/OSDependentd.lib");
					LibraryPaths.Add("lib/Debug/Win64/SPIRVd.lib");
					LibraryPaths.Add("lib/Debug/Win64/MachineIndependentd.lib");
					LibraryPaths.Add("lib/Debug/Win64/GenericCodeGend.lib");
				}
				else
				{
					LibraryPaths.Add("lib/Release/Win64/glslang.lib");
					LibraryPaths.Add("lib/Release/Win64/OGLCompiler.lib");
					LibraryPaths.Add("lib/Release/Win64/OSDependent.lib");
					LibraryPaths.Add("lib/Release/Win64/SPIRV.lib");
					LibraryPaths.Add("lib/Release/Win64/MachineIndependent.lib");
					LibraryPaths.Add("lib/Release/Win64/GenericCodeGen.lib");
				}
			}
		}
	}
}