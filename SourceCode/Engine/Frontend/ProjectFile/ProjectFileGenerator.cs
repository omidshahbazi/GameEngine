// Copyright 2012-2015 ?????????????. All Rights Reserved.
using System.IO;
using System.Collections.Generic;

namespace Frontend
{
	abstract class ProjectFileGenerator
	{
		public enum OutputTypes
		{
			Application,
			DynamicLinkLibrary,
			StaticLinkLibrary
		}

		public enum BuildConfigurations
		{
			Debug = 0,
			Release
		}

		public abstract string AssemblyName
		{
			get;
			set;
		}
		public abstract string OutputPath
		{
			get;
			set;
		}
		public abstract OutputTypes OutputType
		{
			get;
			set;
		}
        public abstract string PreprocessorDefinitions
        {
            get;
            set;
        }
		public abstract string Content
		{
			get;
		}

        public abstract void AddCompileFile(string FilePath);

		public virtual void Generate(string Path)
		{
			File.WriteAllText(Path, Content);
		}
	}
}
