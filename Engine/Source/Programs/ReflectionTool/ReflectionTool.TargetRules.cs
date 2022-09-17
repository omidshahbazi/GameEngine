// Copyright 2016-2020 ?????????????. All Rights Reserved.
#if DEBUG_MODE_PROJECT
namespace Engine.Test
{
	class ReflectionToolTargetRules : TargetRules
	{
		public override string ModuleName
		{
			get { return "ReflectionTool"; }
		}

		public ReflectionToolTargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
		}
	}
}
#endif