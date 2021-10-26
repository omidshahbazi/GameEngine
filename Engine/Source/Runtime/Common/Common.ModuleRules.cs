// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class CommonModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Common"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public CommonModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
		}
	}
}