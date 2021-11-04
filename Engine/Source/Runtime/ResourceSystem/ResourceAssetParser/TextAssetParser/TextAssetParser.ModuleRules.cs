// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class TextAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TextAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public TextAssetParserModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("ResourceCommon");
		}
	}
}