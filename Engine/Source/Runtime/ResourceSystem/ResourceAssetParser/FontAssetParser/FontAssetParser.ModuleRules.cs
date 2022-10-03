// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class FontAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FontAssetParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public FontAssetParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("MeshAssetParser");
			PrivateDependencyModuleNames.Add("TextureAssetParser");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("RenderCommon");
			PublicDependencyModuleNames.Add("MemoryManagement");
		}
	}
}