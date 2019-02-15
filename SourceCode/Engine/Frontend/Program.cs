// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System;
using Engine.Frontend.System.Generator;
using Engine.Frontend.Utilities;
using System;

namespace Engine.Frontend
{
    class Program
    {
        // naming for different configurations
        //clean and rebuild


        static int Main(string[] Args)
        {
			//Args = new string[] { "-BuildEngine", "-x64", "-Debug" };
			//Args = new string[] { "-CleanEngine", "-x64", "-Debug" };
			//Args = new string[] { "-BuildProjectFile" };

			EnvironmentHelper.Initialize();

            ArgumentParser arguments = new ArgumentParser(Args, "-");

            if (arguments.IsDefinedInEnum<BuildSystem.Actions>(0))
            {
                BuildSystem.Actions action = arguments.GetAsEnum<BuildSystem.Actions>(0);

                if (action == BuildSystem.Actions.BuildProjectFile)
                {
                    if (EngineProjectFileCreator.Create())
                        return 0;

#if DEBUG
					Console.Read();
#endif
					return 1;
                }
                else if (arguments.IsDefinedInEnum<BuildSystem.PlatformArchitectures>(1))
                {
                    BuildSystem.PlatformArchitectures architecture = arguments.GetAsEnum<BuildSystem.PlatformArchitectures>(1);

                    if (arguments.IsDefinedInEnum<ProjectBase.ProfileBase.BuildConfigurations>(2))
                    {
                        ProjectBase.ProfileBase.BuildConfigurations buildConfiguration = arguments.GetAsEnum<ProjectBase.ProfileBase.BuildConfigurations>(2);

                        BuildSystem builder = new BuildSystem(action, architecture, buildConfiguration);

						switch (action)
						{
							case BuildSystem.Actions.BuildEngine:
								if (builder.Build())
									return 0;
								break;
							case BuildSystem.Actions.RebuildEngine:
								if (builder.Rebuild())
									return 0;
								break;
							case BuildSystem.Actions.CleanEngine:
								if (builder.Clean())
									return 0;
								break;
						}

#if DEBUG
						Console.Read();
#endif
						return 1;
                    }
                }
            }

            ConsoleHelper.WriteLineError("Parameters should be like -TargetToBuild -PlatformArchitecture -BuildConfiguration");
#if DEBUG
			Console.Read();
#endif
			return 1;
        }
    }
}