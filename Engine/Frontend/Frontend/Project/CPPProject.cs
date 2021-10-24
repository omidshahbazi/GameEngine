// Copyright 2016-2020 ?????????????. All Rights Reserved.

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

            public enum LanguageStandards
            {
                CPP14 = 0,
                CPP17,
                CPPLatest
            }

            private StringList includeDirectories = new StringList();
            private StringList includeLibraries = new StringList();

            public string[] IncludeDirectories
            {
                get { return includeDirectories.ToArray(); }
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

            public bool GenerateDebugInfo
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

            public LanguageStandards LanguageStandard
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

            public Profile(ProjectBase Project) :
                base(Project)
            {
            }

            public void AddIncludeDirectory(string FilePath)
            {
                if (includeDirectories.Contains(FilePath))
                    return;

                includeDirectories.Add(FilePath);
            }

            public void AddIncludeLibrary(string FilePath)
            {
                if (includeLibraries.Contains(FilePath))
                    return;

                includeLibraries.Add(FilePath);
            }
        }

        private StringList includeFiles = new StringList();

        public string[] IncludeFiles
        {
            get { return includeFiles.ToArray(); }
        }

        public void AddIncludeFile(string FilePath)
        {
            includeFiles.Add(FilePath);
        }

        public override ProfileBase CreateProfile()
        {
            Profile profile = new Profile(this);
            AddProfile(profile);
            return profile;
        }
    }
}
