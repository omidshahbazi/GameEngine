// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem
{
	class RenderSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderSystem"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public RenderSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("Threading");
			PrivateDependencyModuleNames.Add("DataUtility");
			PrivateDependencyModuleNames.Add("TimeUtility");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("ResourceCommon");
			PublicDependencyModuleNames.Add("ProgramCompiler");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
			PublicDependencyModuleNames.Add("WindowUtility");
		}
	}
}