// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLEWRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "GLEW"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "GLEW"; }
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
                get { return new string[] { "lib/Release/x64/glew32.lib" }; }
            }

            public override string[] BinariesPath
            {
                get { return new string[] { "bin/Release/x64/glew32.dll" }; }
            }
        }
	}
}