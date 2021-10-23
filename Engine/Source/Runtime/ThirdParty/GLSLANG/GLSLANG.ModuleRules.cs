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

		public class DebugBuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "GLSLANG"; }
			}

			public override Platforms Platform
			{
				get { return Platforms.x64; }
			}

			public override Configurations Configuration
			{
				get { return Configurations.Debug; }
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
				get { return new string[] { "lib/glslangd.lib", "lib/OGLCompilerd.lib", "lib/OSDependentd.lib", "lib/SPIRVd.lib", "lib/MachineIndependentd.lib", "lib/GenericCodeGend.lib" }; }
			}
		}

		public class ReleaseBuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "GLSLANG"; }
			}

			public override Platforms Platform
			{
				get { return Platforms.x64; }
			}

			public virtual Configurations Configuration
			{
				get { return Configurations.Release; }
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
				get { return new string[] { "lib/glslang.lib", "lib/OGLCompiler.lib", "lib/OSDependent.lib", "lib/SPIRV.lib", "lib/SPIRV-Tools.lib", "lib/SPIRV-Tools-opt.lib", "lib/MachineIndependent.lib", "lib/GenericCodeGen.lib" }; }
			}
		}
	}
}