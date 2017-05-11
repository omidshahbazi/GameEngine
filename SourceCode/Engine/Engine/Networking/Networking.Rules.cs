// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class NetworkingRules : BuildRules
	{
		public override string TargetName
		{
			get { return "Networking"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public override string[] DependencyModulesName
		{
			get { return new string[] { "Platform" }; }
		}
	}
}