// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class MathContainersModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "MathContainers"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public MathContainersModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("MemoryManagement");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Platform");
			PublicDependencyModuleNames.Add("Debugging");
			PublicDependencyModuleNames.Add("Mathematics");

			//PreprocessorDefinitions.Add("LEFT_HAND_MATRIX");
		}
	}
}