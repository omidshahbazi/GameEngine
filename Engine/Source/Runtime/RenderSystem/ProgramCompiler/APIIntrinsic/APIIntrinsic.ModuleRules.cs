// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class AAPIIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "APIIntrinsic"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public AAPIIntrinsicModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
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