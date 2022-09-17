// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ThirdParty
{
    class STBModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "STB"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.StaticLibrary; }
		}

		public STBModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
		}
    }
}