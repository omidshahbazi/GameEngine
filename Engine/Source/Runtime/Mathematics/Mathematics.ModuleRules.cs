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

		public MathematicsModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
		}
	}
}