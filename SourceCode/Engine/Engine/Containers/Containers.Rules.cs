// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ContainersRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Containers"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public override string[] DependencyModulesName
		{
			get { return new string[] { "Common", "MemoryManagement" }; }
		}
	}
}