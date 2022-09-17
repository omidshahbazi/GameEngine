// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class AAPIIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "APIIntrinsic"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public AAPIIntrinsicModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");
			PrivateDependencyModuleNames.Add("DataUtility");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("ProgramParser");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
			PublicDependencyModuleNames.Add("Debugging");
		}
	}
}