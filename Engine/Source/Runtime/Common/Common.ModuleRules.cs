// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class CommonModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Common"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public CommonModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
		}
	}
}