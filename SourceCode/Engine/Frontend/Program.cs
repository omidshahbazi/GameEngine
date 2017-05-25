// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.ProjectFile;
using Engine.Frontend.System;
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

		static int Main(string[] Args)
		{
			//FilesMetaDataGenerator.Generate(@"E:\Omid\New folder\SourceCode");

			Args = new string[] { "-Engine", "-X86", "-Debug" };
			//Args = new string[] { "-ProjectFile" };

			if (IsEnumDefine<BuildSystem.TargetsToBuild>(Args[0].Substring(1)))
			{
				BuildSystem.TargetsToBuild toBuild = GetEnum<BuildSystem.TargetsToBuild>(Args[0].Substring(1));

				if (toBuild == BuildSystem.TargetsToBuild.ProjectFile)
				{
					EngineProjectFileCreator.Create();
				}
				else if (Args.Length >= 3 && IsEnumDefine<BuildSystem.PlatformArchitectures>(Args[1].Substring(1)))
				{
					BuildSystem.PlatformArchitectures architecture = GetEnum<BuildSystem.PlatformArchitectures>(Args[1].Substring(1));

					if (IsEnumDefine<ProjectFileBase.BuildConfigurations>(Args[2].Substring(1)))
					{
						ProjectFileBase.BuildConfigurations buildConfiguration = GetEnum<ProjectFileBase.BuildConfigurations>(Args[2].Substring(1));

						BuildSystem builder = new BuildSystem(toBuild, architecture, buildConfiguration);
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