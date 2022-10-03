// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class ImageAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ImageAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ImageAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("STB");
			PrivateDependencyModuleNames.Add("RenderCommon");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("RenderDevice");
		}
	}
}