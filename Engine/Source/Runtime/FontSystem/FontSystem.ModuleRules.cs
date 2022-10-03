// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class FontSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FontSystem"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public FontSystemModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("Platform");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MathContainers");
			PublicDependencyModuleNames.Add("ResourceCommon");
		}
	}
}