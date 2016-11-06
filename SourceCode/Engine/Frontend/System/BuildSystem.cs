// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Engine.Frontend
{
	class BuildSystem
	{
		public enum TargetsToBuild
		{
			Engine = 0,
			Script
		}

		public enum PlatformArchitectures
		{
			X86 = 0,
			X64
		}

		public const string ReflectionToolName = "ReflectionTool";

		private static string processDirectory = "";
		private static string intermediateDirectory = "";
		private static string finalOutputDirectory = "";
		private static bool generateReflection = true;
		private static VCProjectFileGenerator.BuildConfigurations buildConfiguration;
		private static VCProjectFileGenerator.PlatformTypes platformType;
		private static Dictionary<string, SourceBuilder> sourceBuilders = new Dictionary<string, SourceBuilder>();

		private Compiler compiler = new Compiler();
		private List<string> buildRulesPath = new List<string>();
		private Assembly rulesLibrary = null;

		public static string ProcessDirectory
		{
			get { return processDirectory; }
		}

		public static string IntermediateDirectory
		{
			get { return intermediateDirectory; }
		}

		public static string FinalOutputDirectory
		{
			get { return finalOutputDirectory; }
		}

		public static string ReflectionToolPath
		{
			get { return FinalOutputDirectory + ReflectionToolName + ".exe"; }
		}

		public static bool GenerateReflection
		{
			get { return generateReflection; }
		}

		public static VCProjectFileGenerator.PlatformTypes PlatformType
		{
			get { return platformType; }
		}

		public static VCProjectFileGenerator.BuildConfigurations BuildConfiguration
		{
			get { return buildConfiguration; }
			set { buildConfiguration = value; }
		}

		public static Dictionary<string, SourceBuilder> SourceBuilders
		{
			get { return sourceBuilders; }
		}

		public BuildSystem(TargetsToBuild ToBuild, PlatformArchitectures PlatformArchitecture, ProjectFileGenerator.BuildConfigurations BuildConfiguration)
		{
			ConsoleHelper.WriteLineInfo(EnvironmentHelper.Runtime + " under " + EnvironmentHelper.Platform + " is present");

			finalOutputDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + EnvironmentHelper.PathSeparator;
			string rootPath = Path.GetFullPath(finalOutputDirectory + ".." + EnvironmentHelper.PathSeparator);
			processDirectory = rootPath + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator;
			intermediateDirectory = rootPath + "Intermediate" + EnvironmentHelper.PathSeparator;

			platformType = PlatformType;
			buildConfiguration = BuildConfiguration;

			//rootPath = @"D:\Omid\Engine\ge3d\SourceCode";
			//processDirectory = rootPath;
			//intermediateDirectory = rootPath + "Intermediate\\";

			compiler.ErrorRaised += OnError;
		}

		public bool Build()
		{
			if (!BuildRulesLibrary())
				return false;

			BuildSources();

			return true;
		}

		private bool BuildRulesLibrary()
		{
			if (rulesLibrary != null)
			{
				foreach (SourceBuilder builder in sourceBuilders.Values)
					builder.State = SourceBuilder.States.NotBuilt;

				return true;
			}

			const string ProjectName = "Modules";
			string projectDir = intermediateDirectory + ProjectName + EnvironmentHelper.PathSeparator;

			if (!Directory.Exists(projectDir))
				Directory.CreateDirectory(projectDir);

			CSProjectFileGenerator csproj = new CSProjectFileGenerator();
			csproj.FrameworkVersion = CSProjectFileGenerator.FrameworkVersions.v4_5;
			csproj.AssemblyName = ProjectName;
			csproj.OutputPath = projectDir + "Build" + EnvironmentHelper.PathSeparator;
			csproj.OutputType = ProjectFileGenerator.OutputTypes.DynamicLinkLibrary;
			csproj.AddReferenceBinaryFile(Assembly.GetExecutingAssembly().Location);

			string[] files = FileSystemUtilites.GetAllFiles(processDirectory, "*" + BuildRules.FilePostfix);

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteLineInfo("Building rules starts at " + startTime.ToString());
			ConsoleHelper.WriteLineInfo("Found rules :");

			foreach (string rules in files)
			{
				buildRulesPath.Add(rules);
				csproj.AddCompileFile(rules);

				ConsoleHelper.WriteLineInfo("\t" + Path.GetFileName(rules));
			}

			if (buildRulesPath.Count == 0)
			{
				ConsoleHelper.WriteLineInfo("No building rules found, aborting process");
				return false;
			}

			string csprojPath = projectDir + ProjectName + ".csproj";
			csproj.Generate(csprojPath);

			bool wasSuccessful = false;

			if (compiler.BuildProjectFile(csprojPath))
			{
				rulesLibrary = Assembly.LoadFile(csproj.OutputPath + ProjectName + ".dll");
				wasSuccessful = true;
			}

			foreach (string buildRule in buildRulesPath)
			{
				string fileName = Path.GetFileNameWithoutExtension(buildRule);
				string typeName = fileName.Replace(".", "");
				Type type = rulesLibrary.GetType(BuildRules.NamespacePrefix + typeName);

				if (type == null)
				{
					ConsoleHelper.WriteLineWarning("In " + fileName + ", type " + typeName + " doesn't exists, building related module will be ignore");
					continue;
				}

				BuildRules rules = (BuildRules)Activator.CreateInstance(type);

				sourceBuilders[rules.TargetName] = new SourceBuilder(rules, Path.GetDirectoryName(buildRule) + EnvironmentHelper.PathSeparator);
			}

			ConsoleHelper.WriteLineInfo("Building rules takes " + (DateTime.Now - startTime).ToHHMMSS());

			return wasSuccessful;
		}

		private void BuildSources()
		{
			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteLineInfo("Building source starts at " + startTime.ToString());

			for (BuildRules.Priorities priority = BuildRules.Priorities.PreBuildProcess; priority <= BuildRules.Priorities.PostBuildProcess; priority++)
			{
				generateReflection = (priority != BuildRules.Priorities.PreBuildProcess);

				foreach (SourceBuilder builder in sourceBuilders.Values)
					if (builder.Rules.Priority == priority)
						BuildSourceBuilder(builder);
			}

			ConsoleHelper.WriteLineInfo("Building source takes " + (DateTime.Now - startTime).ToHHMMSS());
		}

		private bool BuildSourceBuilder(SourceBuilder Builder)
		{
			if (Builder.Rules.DependencyModulesName != null)
				foreach (string dep in Builder.Rules.DependencyModulesName)
				{
					if (!sourceBuilders.ContainsKey(dep))
					{
						ConsoleHelper.WriteLineWarning("Dependency [" + dep + "] doesn't exists");
						continue;
					}

					SourceBuilder builder = sourceBuilders[dep];

					if (builder == null)
						return false;

					if (!BuildSourceBuilder(builder))
						return false;
				}

			return Builder.Build();
		}

		private void OnError(string Text)
		{
			ConsoleHelper.WriteLineError(Text);
		}
	}
}
