// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class PlatformRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Platform"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public override string[] DependencyModulesName
		{
			get { return new string[] { "Common" }; }
		}

		public override string[] IncludeModulesName
		{
			get { return new string[] { "Common" }; }
		}
	}
}