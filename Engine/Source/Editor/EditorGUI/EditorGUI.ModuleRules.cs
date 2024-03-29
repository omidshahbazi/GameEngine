// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Editor
{
    class EditorGUIModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "EditorGUI"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public EditorGUIModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("WindowUtility");
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("ResourceSystem");
			PrivateDependencyModuleNames.Add("ResourceManagement");
			PrivateDependencyModuleNames.Add("FontSystem");
			PrivateDependencyModuleNames.Add("Debugging");
		}
    }
}