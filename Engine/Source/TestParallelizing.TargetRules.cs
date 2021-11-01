// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
	class TestLauncherTargetRules : TargetRules
	{
		public override string ModuleName
		{
			get { return "TestLauncher"; }
		}

		public TestLauncherTargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
		}
	}
}