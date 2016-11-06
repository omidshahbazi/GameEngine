// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class MemoryManagermentRules : BuildRules
	{
		public override string TargetName
		{
			get { return "MemoryManagerment"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}
	}
}