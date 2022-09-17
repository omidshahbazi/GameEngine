// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.DynamicModuleSystem
{
    class DynamicModuleDefinitionModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "DynamicModuleDefinition"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public DynamicModuleDefinitionModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MemoryManagement");
		}
    }
}