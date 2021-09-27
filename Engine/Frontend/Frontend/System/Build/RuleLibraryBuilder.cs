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
	delegate void NewModuleRuleEventHandler(ModuleRules Rule);
	delegate void NewTargetRuleEventHandler(TargetRules Rule);

	class RuleLibraryBuilder : BuilderBase
	{
		private static readonly RuleLibraryBuilder instance = new RuleLibraryBuilder();

		private bool alreadyBuilt = false;

		private List<ModuleRules> modules = null;
		private List<TargetRules> targets = null;

		public event NewModuleRuleEventHandler OnNewModuleRule;
		public event NewTargetRuleEventHandler OnNewTargetRule;

		protected override string ModuleName
		{
			get { return "Modules"; }
		}

		public ModuleRules[] Modules
		{
			get { return modules.ToArray(); }
		}

		public TargetRules[] Targets
		{
			get { return targets.ToArray(); }
		}

		public static RuleLibraryBuilder Instance
		{
			get { return instance; }
		}

		private RuleLibraryBuilder()
		{
			modules = new List<ModuleRules>();
			targets = new List<TargetRules>();

			Initialize();
		}

		public override void Build(bool ForceToRebuild)
		{
			if (ForceToRebuild)
			{
				alreadyBuilt = false;

				modules.Clear();
				targets.Clear();
			}

			if (alreadyBuilt)
			{
				if (OnNewModuleRule != null)
					for (int i = 0; i < modules.Count; ++i)
						OnNewModuleRule(modules[i]);

				if (OnNewTargetRule != null)
					for (int i = 0; i < targets.Count; ++i)
						OnNewTargetRule(targets[i]);

				return;
			}

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ModuleName;
			profile.OutputPath = IntermediateOutputPath;
			profile.IntermediateDirectory = IntermediateModuleDirectory;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor());
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor());
			csproj.AddReferenceBinaryFile(Assembly.GetExecutingAssembly().Location);

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, "*" + ModuleRules.FilePostfix, "*" + TargetRules.FilePostfix);

			DateTime startTime = DateTime.Now;

			if (files.Length == 0)
				throw new FrontendException("No module rule has found");

			ConsoleHelper.WriteInfo("Found rules :");

			foreach (string rules in files)
			{
				csproj.AddCompileFile(rules);

				ConsoleHelper.WriteInfo("\t" + Path.GetFileName(rules));
			}

			Compiler compiler = new Compiler();
			compiler.ErrorRaised += OnError;

			compiler.Build(profile);

			Assembly rulesLibrary = Assembly.LoadFile(profile.OutputPath + ModuleName + EnvironmentHelper.DynamicLibraryExtentions);

			ExtractModuleRules(rulesLibrary.GetTypes<ModuleRules>());
			ExtractTargetRules(rulesLibrary.GetTypes<TargetRules>());

			ConsoleHelper.WriteInfo($"Building rules took {(DateTime.Now - startTime).ToHHMMSS()}");

			alreadyBuilt = true;
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (!Directory.Exists(IntermediateModuleDirectory))
				Directory.CreateDirectory(IntermediateModuleDirectory);
		}

		private void ExtractModuleRules(Type[] Types)
		{
			foreach (Type type in Types)
			{
				ModuleRules module = (ModuleRules)Activator.CreateInstance(type);

				Type[] types = module.GetType().GetNestedTypes();

				module.BuildRules = new ModuleRules.BuildRulesBase[types.Length];

				for (int i = 0; i < types.Length; ++i)
					module.BuildRules[i] = (ModuleRules.BuildRulesBase)Activator.CreateInstance(types[i]);

				modules.Add(module);

				if (OnNewModuleRule != null)
					OnNewModuleRule(module);
			}
		}

		private void ExtractTargetRules(Type[] Types)
		{
			foreach (Type type in Types)
			{
				TargetRules target = (TargetRules)Activator.CreateInstance(type);

				targets.Add(target);

				if (OnNewTargetRule != null)
					OnNewTargetRule(target);
			}
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteError(Text.Replace("}", "}}"));
		}
	}
}