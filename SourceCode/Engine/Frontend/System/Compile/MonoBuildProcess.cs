// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using System.Diagnostics;
using System.Xml;

namespace Engine.Frontend.System.Compile
{
	//http://www.mono-project.com/docs/about-mono/languages/csharp/
	//http://stackoverflow.com/questions/13664441/how-can-you-use-monos-mcs-to-compile-and-run-a-csproj-file

	class MonoBuildProcess : BuildProcess
	{
		public MonoBuildProcess() :
		base("xbuild")
		{
		}

		public override void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			XmlDocument xml = new XmlDocument ();
			xml.Load (FilePath);

			base.Start("\"" + FilePath + "\" /t:build /p:configuration=" + BuildSystem.BuildConfiguration.ToString().ToLower());
		}
	}
}