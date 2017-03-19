// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class TestLauncherRules : BuildRules
	{
		public override string TargetName
		{
			get { return "TestLauncher"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.Executable; }
		}

		public override string[] DependencyModulesName
		{
			get { return new string[] { "Common", "Debugging", "Platform", "MemoryManagement", "Parallelizing", "Threading"  }; }
		}
	}
}