// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
    class ResourceCommonModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceCommon"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ResourceCommonModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
		}
    }
}