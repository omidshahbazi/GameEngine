// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ParallelizingRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Parallelizing"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public override string[] DependencyModulesName
		{
			get { return new string[] { "Platform", "MemoryManagement", "Threading" }; }
		}
	}
}