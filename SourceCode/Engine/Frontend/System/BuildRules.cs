// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    public abstract class BuildRules
    {
		public enum LibraryUseTypes
        {
            Executable = 0,
            DynamicLibrary,
            StaticLibrary,
			ProjectFile,
			UseOnly
        }

		public enum Priorities
		{
			PreBuildProcess = 0,
			InBuildProcess,
			PostBuildProcess
		}

		public const string FilePostfix = ".Rules.cs";
		public const string NamespacePrefix = "Engine.Frontend.";

		public abstract string TargetName
        {
            get;
        }

		public abstract LibraryUseTypes LibraryUseType
        {
            get;
        }

		public virtual string ProjectFilePath
		{
			get { return string.Empty; }
		}

		public virtual string[] OutputsFilePath
		{
			get { return null; }
		}

		public virtual string[] PreprocessorDefinitions
		{
			get { return null; }
		}

        public virtual string[] DependencyModulesName
        {
            get { return null; }
        }

		public virtual string[] DependencyStaticLibraries
		{
			get { return null; }
		}

		public virtual string[] IncludeModulesName
		{
			get { return null; }
		}

		public virtual string[] AdditionalCompileFile
		{
			get { return null; }
		}

		public virtual Priorities Priority
		{
			get { return Priorities.InBuildProcess; }
		}
    }
}