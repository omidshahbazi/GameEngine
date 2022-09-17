// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ParallelizingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Parallelizing"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ParallelizingModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("Platform");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("Threading");
		}
	}
}