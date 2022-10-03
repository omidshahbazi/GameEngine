// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class TTFAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TTFAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public TTFAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("FreeType");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("MemoryManagement");

			DependencyStaticLibraries.Add("opengl32.lib");
			DependencyStaticLibraries.Add("Glu32.lib");
		}
	}
}