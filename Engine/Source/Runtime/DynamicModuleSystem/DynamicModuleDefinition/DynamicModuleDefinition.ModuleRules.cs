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

		public DynamicModuleDefinitionModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MemoryManagement");
		}
    }
}