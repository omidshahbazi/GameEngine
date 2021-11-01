// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
	class TestParallelizingTargetRules : TargetRules
	{
		public override string ModuleName
		{
			get { return "TestParallelizing"; }
		}

		public TestParallelizingTargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
		}
	}
}