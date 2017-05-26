// Copyright 2016-2017 ?????????????. All Rights Reserved.

namespace Engine.Frontend.Project
{
	class CPPProject : ProjectBase
	{
		public class Profile : ProfileBase
		{
			public enum RuntimeLibraries
			{
				MultiThreaded = 0,
				MultiThreadedDebug,
				MultiThreadedDLL,
				MultiThreadedDebugDLL
			}

			public enum Optimizations
			{
				Disabled = 0,
				MaxSize,
				MaxSpeed,
				FullOptimization
			}

			private StringList additionalLibraryDirectories = new StringList();
			private StringList includeDirectories = new StringList();
			private StringList includeLibraryDirectories = new StringList();
			private StringList includeLibraries = new StringList();

			public string[] AdditionalLibraryDirectories
			{
				get { return additionalLibraryDirectories.ToArray(); }
			}

			public string[] IncludeDirectories
			{
				get { return includeDirectories.ToArray(); }
			}

			public string[] IncludeLibraryDirectories
			{
				get { return includeLibraryDirectories.ToArray(); }
			}

			public string[] IncludeLibraries
			{
				get { return includeLibraries.ToArray(); }
			}

			public RuntimeLibraries RuntimeLibrary
			{
				get;
				set;
			}

			public bool GenerateDebugInformation
			{
				get;
				set;
			}

			public Optimizations Optimization
			{
				get;
				set;
			}

			public bool MinimalRebuild
			{
				get;
				set;
			}

			public string NMakeBuildCommandLine
			{
				get;
				set;
			}

			public string NMakeReBuildCommandLine
			{
				get;
				set;
			}

			public string NMakeCleanCommandLine
			{
				get;
				set;
			}

			public void AddAdditionalLibraryDirectories(string FilePath)
			{
				additionalLibraryDirectories.Add(FilePath);
			}

			public void AddIncludeDirectories(string FilePath)
			{
				includeDirectories.Add(FilePath);
			}

			public void AddIncludeLibraryDirectories(string FilePath)
			{
				includeLibraryDirectories.Add(FilePath);
			}

			public void AddIncludeLibraries(string FilePath)
			{
				includeLibraries.Add(FilePath);
			}
		}

		private StringList includeFiles = new StringList();

		public string[] IncludeFiles
		{
			get { return includeFiles.ToArray(); }
		}

		public override ProfileBase CreateProfile()
		{
			Profile profile = new Profile();
			AddProfile(profile);
			return profile;
		}

		public void AddIncludeFile(string FilePath)
		{
			includeFiles.Add(FilePath);
		}
	}
}
