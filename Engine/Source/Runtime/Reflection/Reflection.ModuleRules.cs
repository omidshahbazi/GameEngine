// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ReflectionModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Reflection"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Reflection"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "Platform", "Debugging" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "MathContainers" }; }
			}
		}
	}
}