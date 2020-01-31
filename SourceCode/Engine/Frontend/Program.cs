// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System;
using Engine.Frontend.System.Generator;
using GameFramework.Common.Utilities;
using System;

namespace Engine.Frontend
{
	class Program
	{
		// naming for different configurations

		static int Main(string[] Args)
		{
			EnvironmentHelper.Initialize();

			ArgumentParser arguments = ArgumentParser.Parse(Args);

			if (arguments.Contains("Action"))
			{
				BuildSystem.Actions action = arguments.Get<BuildSystem.Actions>("Action");

				if (action == BuildSystem.Actions.BuildProjectFile)
				{
					if (EngineProjectFileCreator.Create())
						return 0;
#if DEBUG
					Console.Read();
#endif
					return 1;
				}
				else if (arguments.Contains("Architecture"))
				{
					ProjectBase.ProfileBase.PlatformArchitectures architecture = arguments.Get<ProjectBase.ProfileBase.PlatformArchitectures>("Architecture");

					if (arguments.Contains("Configuration"))
					{
						ProjectBase.ProfileBase.BuildConfigurations buildConfiguration = arguments.Get<ProjectBase.ProfileBase.BuildConfigurations>("Configuration");

						BuildSystem builder = new BuildSystem(architecture, buildConfiguration);

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

			ConsoleHelper.WriteError("Parameters should be like -Action {Param} -Architecture {Param} -Configuration {Param}");
#if DEBUG
			Console.Read();
#endif
			return 1;
		}
	}
}