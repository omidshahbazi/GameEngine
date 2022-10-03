// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class MeshAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "MeshAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public MeshAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("RenderCommon");
		}
	}
}