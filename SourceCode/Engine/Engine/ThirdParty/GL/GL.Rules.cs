// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "GL"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "GL"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
            }
		}
	}
}