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
	class RulesLibraryBuilder : BaseBuilder
	{
		private class TypeList : List<Type>
		{
		}

		private bool alreadyBuilt = false;

		private TypeList moduleTypes = null;
		private TypeList targetTypes = null;

		public override string ModuleName
		{
			get { return "Modules"; }
		}

		public Type[] ModuleTupes
		{
			get { return moduleTypes.ToArray(); }
		}

		public Type[] TargetTypes
		{
			get { return targetTypes.ToArray(); }
		}

		public RulesLibraryBuilder(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture) :
			base(Configuration, Architecture)
		{
			moduleTypes = new TypeList();
			targetTypes = new TypeList();

			Initialize();
		}

		public override void Build(bool ForceToRebuild)
		{
			if (ForceToRebuild)
			{
				alreadyBuilt = false;

				moduleTypes.Clear();
				targetTypes.Clear();
			}

			if (alreadyBuilt)
				return;

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v3_5;
			profile.AssemblyName = ModuleName;
			profile.OutputPath = IntermediateOutputPath;
			profile.IntermediateDirectory = ModuleName.GetIntermediateDirectory();
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			profile.AddPreprocessorDefinition(Configuration.GetPreprocessor());
			profile.AddPreprocessorDefinition(Architecture.GetPreprocessor());
			profile.AddPreprocessorDefinition(EnvironmentHelper.OperatingSystem.GetPreprocessor());
			csproj.AddReferenceBinaryFile(EnvironmentHelper.FrontenddToolPath);

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, $"*{ModuleRules.FilePostfix}", $"*{TargetRules.FilePostfix}");

			DateTime startTime = DateTime.Now;

			if (files.Length == 0)
				throw new FrontendException("No module rules has found");

			ConsoleHelper.WriteInfo("Found rules :");

			foreach (string rules in files)
			{
				csproj.AddCompileFile(rules);

				ConsoleHelper.WriteInfo($"\t{Path.GetFileName(rules)}");
			}

			Compiler compiler = new Compiler();
			compiler.ErrorRaised += OnError;

			compiler.Build(profile);

			Assembly rulesLibrary = Assembly.LoadFile(profile.OutputPath + ModuleName + EnvironmentHelper.DynamicLibraryExtensions);

			moduleTypes.AddRange(rulesLibrary.GetTypes<ModuleRules>());
			targetTypes.AddRange(rulesLibrary.GetTypes<TargetRules>());

			ConsoleHelper.WriteInfo($"Building rules took {(DateTime.Now - startTime).ToHHMMSS()}");

			alreadyBuilt = true;
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (!Directory.Exists(IntermediateModuleDirectory))
				Directory.CreateDirectory(IntermediateModuleDirectory);
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteError(Text.Replace("{", "{{").Replace("}", "}}"));
		}
	}
}