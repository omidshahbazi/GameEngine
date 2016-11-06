// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;

namespace Engine.Frontend
{
    class Program
    {
        static int Main(string[] Args)
        {
            //FilesMetaDataGenerator.Generate(@"E:\Omid\New folder\SourceCode");

            Args = new string[] { "-Engine", "-X86", "-Debug" };

            if (Args.Length >= 3)
            {
                if (Enum.IsDefined(typeof(BuildSystem.TargetsToBuild), Args[0].Substring(1)))
                {
                    BuildSystem.TargetsToBuild toBuild = (BuildSystem.TargetsToBuild)Enum.Parse(typeof(BuildSystem.TargetsToBuild), Args[0].Substring(1));

                    if (Enum.IsDefined(typeof(BuildSystem.PlatformArchitectures), Args[1].Substring(1)))
                    {
                        BuildSystem.PlatformArchitectures architecture = (BuildSystem.PlatformArchitectures)Enum.Parse(typeof(BuildSystem.PlatformArchitectures), Args[1].Substring(1));

                        if (Enum.IsDefined(typeof(ProjectFileGenerator.BuildConfigurations), Args[2].Substring(1)))
                        {
                            ProjectFileGenerator.BuildConfigurations buildConfiguration = (ProjectFileGenerator.BuildConfigurations)Enum.Parse(typeof(ProjectFileGenerator.BuildConfigurations), Args[2].Substring(1));

                            BuildSystem builder = new BuildSystem(toBuild, architecture, buildConfiguration);
                            if (builder.Build())
                                return 0;

                            return 1;
                        }
                    }
                }
            }

            ConsoleHelper.WriteLineError("Parameters should be like -TargetToBuild -PlatformArchitecture -BuildConfiguration");
            return 1;
        }
    }
}