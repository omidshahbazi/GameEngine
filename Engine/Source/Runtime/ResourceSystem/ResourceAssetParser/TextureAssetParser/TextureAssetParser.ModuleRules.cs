// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class TextureAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TextureAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public TextureAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("STB");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("RenderCommon");
		}
	}
}