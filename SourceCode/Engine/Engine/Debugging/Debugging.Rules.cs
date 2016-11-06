// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class DebuggingRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Debugging"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}
	}
}