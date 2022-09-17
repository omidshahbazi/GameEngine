// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
    class TestParallelizingModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestParallelizing"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.Executable; }
		}

		public TestParallelizingModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("Parallelizing");
			PrivateDependencyModuleNames.Add("Threading");

			IconPath = "Engine/Contents/Icon.png";
		}
    }
}