// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace Engine.Frontend.System.Build
{
	delegate void NewBuildRuleEventHandler(string FilePath, BuildRules Rule);

	class RuleLibraryBuilder : BuilderBase
	{
		private const string ProjectName = "Modules";

		public event NewBuildRuleEventHandler OnNewBuildRule;

		public RuleLibraryBuilder() : base(ProjectName)
		{
		}

		public override bool Build(bool ForceToRebuild)
		{
			//if (!Directory.Exists(IntermediateModulePath))
			//	Directory.CreateDirectory(IntermediateModulePath);

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = BinariesPath;
			profile.IntermediatePath = IntermediateModulePath;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			csproj.AddReferenceBinaryFile(Assembly.GetExecutingAssembly().Location);

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.ProcessDirectory, "*" + BuildRules.BuildRuleFilePostfix);

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building rules starts at " + startTime.ToString());
			ConsoleHelper.WriteInfo("Found rules :");

			if (files.Length == 0)
			{
				ConsoleHelper.WriteInfo("No building rules found, aborting process");
				return false;
			}

			foreach (string rules in files)
			{
				csproj.AddCompileFile(rules);

				ConsoleHelper.WriteInfo("\t" + Path.GetFileName(rules));
			}

			Compiler compiler = new Compiler();
			compiler.ErrorRaised += OnError;

			if (compiler.Build(profile))
			{
				Assembly rulesLibrary = Assembly.LoadFile(profile.OutputPath + ProjectName + EnvironmentHelper.DynamicLibraryExtentions);

				foreach (string buildRuleName in files)
				{
					string fileName = Path.GetFileNameWithoutExtension(buildRuleName);
					string typeName = fileName.Replace(".", "");
					Type type = rulesLibrary.GetType(BuildRules.NamespacePrefix + typeName);

					if (type == null)
					{
						ConsoleHelper.WriteWarning("In " + fileName + ", type " + typeName + " doesn't exists, building related module will be ignore");
						continue;
					}

					BuildRules buildRule = (BuildRules)Activator.CreateInstance(type);
					buildRule.Path = FileSystemUtilites.PathSeperatorCorrection(Path.GetDirectoryName(buildRuleName)) + EnvironmentHelper.PathSeparator;

					Type[] types = buildRule.GetType().GetNestedTypes();

					buildRule.Rules = new BuildRules.RuleBase[types.Length];

					for (int i = 0; i < types.Length; ++i)
						buildRule.Rules[i] = (BuildRules.RuleBase)Activator.CreateInstance(types[i]);

					if (OnNewBuildRule != null)
						OnNewBuildRule(buildRuleName, buildRule);
				}

				ConsoleHelper.WriteInfo("Building rules takes " + (DateTime.Now - startTime).ToHHMMSS());

				return true;
			}

			return false;
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteError(Text);
		}
	}
}