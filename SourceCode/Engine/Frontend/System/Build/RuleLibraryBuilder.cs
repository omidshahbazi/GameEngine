// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace Engine.Frontend.System.Build
{
	class RuleLibraryBuilder
	{
		private const string ProjectName = "Modules";

		private string processDirectory = string.Empty;

		public string[] RulesFiles
		{
			get;
			private set;
		}

		public BuildRules[] Rules
		{
			get;
			private set;
		}

		public RuleLibraryBuilder(string ProcessDirectory)
		{

			processDirectory = ProcessDirectory;
		}

		public bool Build()
		{
			string projectDir = EnvironmentHelper.IntermediateDirectory + ProjectName + EnvironmentHelper.PathSeparator;

			if (!Directory.Exists(projectDir))
				Directory.CreateDirectory(projectDir);

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = projectDir + "Build" + EnvironmentHelper.PathSeparator;
			profile.IntermediatePath = projectDir;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			csproj.AddReferenceBinaryFile(Assembly.GetExecutingAssembly().Location);

			string[] files = FileSystemUtilites.GetAllFiles(processDirectory, "*" + BuildRules.BuildRuleFilePostfix);

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteLineInfo("Building rules starts at " + startTime.ToString());
			ConsoleHelper.WriteLineInfo("Found rules :");

			if (files.Length == 0)
			{
				ConsoleHelper.WriteLineInfo("No building rules found, aborting process");
				return false;
			}

			foreach (string rules in files)
			{
				csproj.AddCompileFile(rules);

				ConsoleHelper.WriteLineInfo("\t" + Path.GetFileName(rules));
			}

			Compiler compiler = new Compiler();
			compiler.ErrorRaised += OnError;

			if (compiler.Build(profile))
			{
				Assembly rulesLibrary = Assembly.LoadFile(profile.OutputPath + ProjectName + EnvironmentHelper.DynamicLibraryExtentions);

				List<string> buildRulesFiles = new List<string>();
				List<BuildRules> buildRules = new List<BuildRules>();

				foreach (string buildRule in files)
				{
					string fileName = Path.GetFileNameWithoutExtension(buildRule);
					string typeName = fileName.Replace(".", "");
					Type type = rulesLibrary.GetType(BuildRules.NamespacePrefix + typeName);

					if (type == null)
					{
						ConsoleHelper.WriteLineWarning("In " + fileName + ", type " + typeName + " doesn't exists, building related module will be ignore");
						continue;
					}

					buildRulesFiles.Add(buildRule);
					buildRules.Add((BuildRules)Activator.CreateInstance(type));
				}

				RulesFiles = buildRulesFiles.ToArray();
				Rules = buildRules.ToArray();

				return true;
			}

			ConsoleHelper.WriteLineInfo("Building rules takes " + (DateTime.Now - startTime).ToHHMMSS());

			return false;
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteLineError(Text);
		}
	}
}
