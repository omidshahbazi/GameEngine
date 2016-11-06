// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class CommonRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Common"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}
	}
}