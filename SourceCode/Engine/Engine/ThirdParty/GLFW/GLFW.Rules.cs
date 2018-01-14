// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLFWRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "GLFW"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "GLFW"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
            }

            public override string[] IncludesPath
			{
				get { return new string[] { "include/" }; }
            }

            public override string[] LibrariesPath
            {
                get { return new string[] { "lib/glfw3.lib" }; }
            }
        }
	}
}