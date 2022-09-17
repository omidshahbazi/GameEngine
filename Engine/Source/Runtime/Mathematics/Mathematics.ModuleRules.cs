// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class MathematicsModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Mathematics"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public MathematicsModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
		}
	}
}