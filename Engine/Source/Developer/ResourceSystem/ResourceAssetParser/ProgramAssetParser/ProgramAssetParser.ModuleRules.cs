// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class ProgramAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ProgramAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderDevice");
		}
	}
}