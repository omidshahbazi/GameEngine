// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
    class ResourceImportExportModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceImportExport"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ResourceImportExportModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("JSON");
			PrivateDependencyModuleNames.Add("Allocators");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("Platform");

			GenerateReflection = true;
		}
    }
}