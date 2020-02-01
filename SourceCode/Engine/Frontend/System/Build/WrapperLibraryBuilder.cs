// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;

namespace Engine.Frontend.System.Build
{
	class WrapperLibraryBuilder : BuilderBase
	{
		public class WrapperFileList : List<string>
		{ }

		private const string ProjectName = "Wrapper";

		public WrapperFileList WrapperFiles
		{
			get;
			private set;
		}

		public WrapperLibraryBuilder() : base(ProjectName)
		{
			WrapperFiles = new WrapperFileList();
		}

		public override bool Build(bool ForceToRebuild)
		{
			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = BinariesPath;
			profile.IntermediatePath = IntermediateModulePath;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			profile.PlatformArchitecture = BuildSystem.PlatformArchitecture;

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building wrapper starts at " + startTime.ToString());

			if (WrapperFiles.Count == 0)
			{
				ConsoleHelper.WriteInfo("No building rules found, aborting process");
				return false;
			}

			foreach (string file in WrapperFiles)
				csproj.AddCompileFile(file);

			if (Compiler.Build(profile))
			{
				CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				ConsoleHelper.WriteInfo("Building wrapper takes " + (DateTime.Now - startTime).ToHHMMSS());

				return true;
			}

			return false;
		}
	}
}
