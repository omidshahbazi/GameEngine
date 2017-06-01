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
			public enum PlatformTypes
			{
				x86 = 0,
				x64
			}

			public enum OutputTypes
			{
				Application,
				DynamicLinkLibrary,
				StaticLinkLibrary,
				Makefile
			}

			public enum BuildConfigurations
			{
				Debug = 0,
				Release
			}

			private StringList preprocessorDefinitions = new StringList();

			public virtual ProjectBase Project
			{
				get;
				private set;
			}

			public string Name
			{
				get;
				set;
			}

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

			public virtual string IntermediatePath
			{
				get;
				set;
			}

			//public virtual string WorkingDirectory
			//{
			//	get;
			//	set;
			//}

			public PlatformTypes PlatformType
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

			public ProfileBase(ProjectBase Project)
			{
				this.Project = Project;
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
		private StringList extraFiles = new StringList();

		public virtual ProfileBase[] Profiles
		{
			get { return profiles.ToArray(); }
		}

		public virtual string[] CompileFiles
		{
			get { return compileFiles.ToArray(); }
		}

		public virtual string[] ExtraFiles
		{
			get { return extraFiles.ToArray(); }
		}

		public abstract ProfileBase CreateProfile();

		public virtual void AddCompileFile(string FilePath)
		{
			compileFiles.Add(FilePath);
		}

		public virtual void AddExtraFile(string FilePath)
		{
			extraFiles.Add(FilePath);
		}

		protected virtual void AddProfile(ProfileBase Profile)
		{
			profiles.Add(Profile);
		}
	}
}
