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
	delegate void NewBuildRuleEventHandler(string FilePath, ModuleRules Rule);

	class RuleLibraryBuilder : BuilderBase
	{
		private static readonly RuleLibraryBuilder instance = new RuleLibraryBuilder();

		private const string ProjectName = "Modules";

		private List<string> buildRulesPath = null;
		private List<ModuleRules> buildRules = null;

		public event NewBuildRuleEventHandler OnNewBuildRule;

		public static RuleLibraryBuilder Instance
		{
			get { return instance; }
		}

		private RuleLibraryBuilder() : base(ProjectName)
		{
			buildRulesPath = new List<string>();
			buildRules = new List<ModuleRules>();
		}

		public override bool Build(bool ForceToRebuild)
		{
			if (ForceToRebuild)
			{
				buildRulesPath.Clear();
				buildRules.Clear();
			}

			if (buildRules.Count != 0)
			{
				for (int i = 0; i < buildRulesPath.Count; ++i)
					OnNewBuildRule(buildRulesPath[i], buildRules[i]);

				return true;
			}

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = IntermediateBinariesPath;
			profile.IntermediatePath = IntermediateModulePath;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			csproj.AddReferenceBinaryFile(Assembly.GetExecutingAssembly().Location);

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.ProcessDirectory, "*" + ModuleRules.ModuleRuleFilePostfix);

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
					Type type = rulesLibrary.GetType(ModuleRules.NamespacePrefix + typeName);

					if (type == null)
					{
						ConsoleHelper.WriteWarning("In " + fileName + ", type " + typeName + " doesn't exists, building related module will be ignore");
						continue;
					}

					ModuleRules buildRule = (ModuleRules)Activator.CreateInstance(type);
					buildRule.Path = FileSystemUtilites.PathSeperatorCorrection(Path.GetDirectoryName(buildRuleName)) + EnvironmentHelper.PathSeparator;

					Type[] types = buildRule.GetType().GetNestedTypes();

					buildRule.BuildRules = new ModuleRules.BuildRulesBase[types.Length];

					for (int i = 0; i < types.Length; ++i)
						buildRule.BuildRules[i] = (ModuleRules.BuildRulesBase)Activator.CreateInstance(types[i]);

					if (OnNewBuildRule != null)
						OnNewBuildRule(buildRuleName, buildRule);

					buildRulesPath.Add(buildRuleName);
					buildRules.Add(buildRule);
				}

				ConsoleHelper.WriteInfo("Building rules takes " + (DateTime.Now - startTime).ToHHMMSS());

				return true;
			}

			return false;
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (!Directory.Exists(IntermediateModulePath))
				Directory.CreateDirectory(IntermediateModulePath);
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteError(Text);
		}
	}
}