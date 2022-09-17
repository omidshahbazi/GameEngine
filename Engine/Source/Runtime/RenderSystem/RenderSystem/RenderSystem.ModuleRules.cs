// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem
{
	class RenderSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderSystem"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public RenderSystemModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
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