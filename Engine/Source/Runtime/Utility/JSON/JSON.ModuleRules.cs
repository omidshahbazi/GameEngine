// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class JSONModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "JSON"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public JSONModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Lexer");
			PrivateDependencyModuleNames.Add("MemoryManagement");
		}
	}
}