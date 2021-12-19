// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.MemoryManagement
{
	class AllocatorsModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Allocators"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public AllocatorsModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Platform");

			PublicDependencyModuleNames.Add("Common");

			if (Platform == Platforms.x86)
				PreprocessorDefinitions.Add("ONLY_USING_C_ALLOCATOR");
			else
			{
				PreprocessorDefinitions.Add("USE_VIRTUAL_ADDRESS_SPACE");
				PreprocessorDefinitions.Add("LEAK_DETECTION");
				PreprocessorDefinitions.Add("CORRUPTED_HEAP_DETECTION");
			}


			PreprocessorDefinitions.Add("ONLY_USING_C_ALLOCATOR");
			PreprocessorDefinitions.Remove("USE_VIRTUAL_ADDRESS_SPACE");
		}
	}
}