// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Collections.Generic;

namespace Engine.Frontend.Project
{
	abstract class ProjectBase
	{
		protected class StringList : List<string>
		{ }

		public abstract class ProfileBase
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

			public virtual void AddPreprocessorDefinition(string Preprocessor)
			{
				preprocessorDefinitions.Add(Preprocessor);
			}
		}

		protected class ProfileList : List<ProfileBase>
		{ }

		private ProfileList profiles = new ProfileList();
		private StringList compileFiles = new StringList();

		public virtual ProfileBase[] Profiles
		{
			get { return profiles.ToArray(); }
		}

		public virtual string[] CompileFiles
		{
			get { return compileFiles.ToArray(); }
		}

		public abstract ProfileBase CreateProfile();

		public virtual void AddCompileFile(string FilePath)
		{
			compileFiles.Add(FilePath);
		}

		protected virtual void AddProfile(ProfileBase Profile)
		{
			profiles.Add(Profile);
		}
	}
}
