// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class OBJAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "OBJAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public OBJAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderDevice");
		}
	}
}