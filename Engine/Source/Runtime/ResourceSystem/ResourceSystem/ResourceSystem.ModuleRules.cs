// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
	class ResourceSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ResourceSystem"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ResourceSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("ResourceCommon");
			PrivateDependencyModuleNames.Add("ResourceImportExport");
			PrivateDependencyModuleNames.Add("ProgramCompiler");
			PrivateDependencyModuleNames.Add("ImageAssetParser");
			PrivateDependencyModuleNames.Add("TextureAssetParser");
			PrivateDependencyModuleNames.Add("MeshAssetParser");
			PrivateDependencyModuleNames.Add("TTFAssetParser");
			PrivateDependencyModuleNames.Add("FontAssetParser");
			PrivateDependencyModuleNames.Add("OBJAssetParser");
			PrivateDependencyModuleNames.Add("TextAssetParser");
			PrivateDependencyModuleNames.Add("ProgramAssetParser");
			PrivateDependencyModuleNames.Add("CompiledProgramAssetParser");
			PrivateDependencyModuleNames.Add("TTFAssetParser");
			PrivateDependencyModuleNames.Add("FontAssetParser");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("JSON");
			PrivateDependencyModuleNames.Add("DataUtility");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("FileUtility");
			PublicDependencyModuleNames.Add("Threading");
		}
	}
}