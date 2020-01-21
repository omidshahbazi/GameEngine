// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using System.Diagnostics;
using System.Xml;
using System;

namespace Engine.Frontend.System.Compile
{
	//http://www.mono-project.com/docs/about-mono/languages/csharp/
	//http://stackoverflow.com/questions/13664441/how-can-you-use-monos-mcs-to-compile-and-run-a-csproj-file

	class MonoBuildProcess : BuildProcess
	{
		public override string FilePath
		{
			get { return "xbuild"; }
		}

		public override void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
			//base.Start("\"" + FilePath + "\" /t:build /p:configuration=" + BuildSystem.BuildConfiguration.ToString().ToLower());
		}

		public override void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			//XmlDocument xml = new XmlDocument ();
			//xml.Load (FilePath);

		}

		public override void Rebuild(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
		}

		public override void Clean(string ProjectPath)
		{
		}
	}
}