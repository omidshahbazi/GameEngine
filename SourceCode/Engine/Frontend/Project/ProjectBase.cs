// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Collections.Generic;

namespace Engine.Frontend.Project
{
	abstract class ProjectBase
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

		protected class StringList : List<string>
		{ }

		private StringList compileFiles = new StringList();
		private StringList preprocessorDefinitions = new StringList();

		public virtual string AssemblyName
		{
			get;
			set;
		}

		public virtual string OutputPath
		{
			get;
			set;
		}

		public BuildConfigurations BuildConfiguration
		{
			get;
			set;
		}

		public virtual OutputTypes OutputType
		{
			get;
			set;
		}

		public virtual string[] PreprocessorDefinitions
		{
			get { return preprocessorDefinitions.ToArray(); }
		}

		public virtual string[] CompileFiles
		{
			get { return compileFiles.ToArray(); }
		}

		public virtual void AddPreprocessorDefinition(string Preprocessor)
		{
			preprocessorDefinitions.Add(Preprocessor);
		}

		public virtual void AddCompileFile(string FilePath)
		{
			compileFiles.Add(FilePath);
		}
	}
}
