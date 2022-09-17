// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ReflectionModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Reflection"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ReflectionModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("Debugging");

			PublicDependencyModuleNames.Add("MathContainers");
		}
	}
}