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
		private static bool IsEnumDefine<T>(string Name)
		{
			return Enum.IsDefined(typeof(T), Name);
		}

		private static T GetEnum<T>(string Name)
		{
			return (T)Enum.Parse(typeof(T), Name);
		}

		// make generic argument parser
		// make sure that frontend is cross platform
		// naming for different configurations

		static int Main(string[] Args)
		{
			EnvironmentHelper.Initialize();

			Args = new string[] { "-BuildEngine", "-x64", "-Debug" };
			//Args = new string[] { "-BuildProjectFile" };

			if (Args.Length != 0 && IsEnumDefine<BuildSystem.Actions>(Args[0].Substring(1)))
			{
				BuildSystem.Actions action = GetEnum<BuildSystem.Actions>(Args[0].Substring(1));

				if (action == BuildSystem.Actions.BuildProjectFile)
				{
					EngineProjectFileCreator.Create();
					return 0;
				}
				else if (Args.Length >= 3 && IsEnumDefine<BuildSystem.PlatformArchitectures>(Args[1].Substring(1)))
				{
					BuildSystem.PlatformArchitectures architecture = GetEnum<BuildSystem.PlatformArchitectures>(Args[1].Substring(1));

					if (IsEnumDefine<ProjectBase.ProfileBase.BuildConfigurations>(Args[2].Substring(1)))
					{
						ProjectBase.ProfileBase.BuildConfigurations buildConfiguration = GetEnum<ProjectBase.ProfileBase.BuildConfigurations>(Args[2].Substring(1));

						BuildSystem builder = new BuildSystem(action, architecture, buildConfiguration);
						if (builder.Build())
							return 0;

						return 1;
					}
				}
			}

			ConsoleHelper.WriteLineError("Parameters should be like -TargetToBuild -PlatformArchitecture -BuildConfiguration");
			return 1;
		}
	}
}