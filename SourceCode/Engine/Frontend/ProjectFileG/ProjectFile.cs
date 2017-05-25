// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Engine.Frontend.ProjectFileGenerator
{
	abstract class ProjectFile
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

		public abstract string Content
		{
			get;
		}

		public virtual void AddPreprocessorDefinition(string Preprocessor)
		{
			preprocessorDefinitions.Add(Preprocessor);
		}

		public virtual void AddCompileFile(string FilePath)
		{
			compileFiles.Add(FilePath);
		}

		protected static string GetFlattenStringList(IEnumerable<string> List, string Separator)
		{
			StringBuilder value = new StringBuilder();

			IEnumerator<string> it = List.GetEnumerator();

			while (it.MoveNext())
			{
				value.Append(it.Current);
				value.Append(Separator);
			}

			return value.ToString();
		}
	}
}
