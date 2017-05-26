// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.Utilities;
using System;
using System.IO;

namespace Engine.Frontend.System
{
	class SourceBuilder
	{
		public enum States
		{
			NotBuilt = 0,
			Built,
			Failed
		}

		private const string BinariesFolderName = "Binaries";
		private const string ExeExtension = ".exe";
		private const string DLLExtension = ".dll";
		private const string LibExtension = ".lib";

		private BuildRules rules = null;
		private string sourcePathRoot = "";

		private States state = States.NotBuilt;

		private string intermediateModulePath = "";
		private string generatedFilesPath = "";
		private Compiler compiler = null;
		private static CommandLineProcess reflectionGeneratorProcess = null;

		private string BinariesPath
		{
			//get { return (rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly ? sourcePathRoot : intermediateModulePath) + BinariesFolder; }
			get { return intermediateModulePath + BinariesFolderName + EnvironmentHelper.PathSeparator; }
		}

		public BuildRules Rules
		{
			get { return rules; }
		}

		public States State
		{
			get { return state; }
			set { state = value; }
		}

		public SourceBuilder(BuildRules Rules, string SourcePathRoot)
		{
			rules = Rules;
			sourcePathRoot = SourcePathRoot;

			intermediateModulePath = BuildSystem.IntermediateDirectory + Rules.TargetName + EnvironmentHelper.PathSeparator;
			generatedFilesPath = intermediateModulePath + "Generated" + EnvironmentHelper.PathSeparator;

			if (rules.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
			{
				//if (Directory.Exists(intermediateModulePath))
				//	Directory.Delete(intermediateModulePath, true);
				//Directory.CreateDirectory(intermediateModulePath);

				if (!Directory.Exists(generatedFilesPath))
					Directory.CreateDirectory(generatedFilesPath);
			}

			compiler = new Compiler();
			compiler.ErrorRaised += ErrorRaised;
		}

		public bool Build()
		{
			if (state == States.Built)
				return true;
			else if (state == States.Failed)
				return false;

			if (rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
				state = States.Built;
			else if (rules.LibraryUseType == BuildRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile();

			return (state == States.Built);
		}

		private void LogInfo()
		{
			LogHelper.DeleteLog(rules.TargetName);
			ConsoleHelper.WriteLineInfo("Building " + rules.TargetName);
		}

		private void GenerateAndBuildProjectFile()
		{
			LogInfo();

			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = rules.TargetName;
			profile.OutputType = LibraryUseTypesToOutputType(rules.LibraryUseType);
			profile.OutputPath = BinariesPath;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = true;
			profile.PlatformType = BuildSystem.PlatformType;
			profile.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (profile.BuildConfiguration == CPPProject.Profile.BuildConfigurations.Debug)
			{
				profile.GenerateDebugInformation = true;

				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDebug;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else
				profile.GenerateDebugInformation = false;
			{
				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreaded;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			profile.AddIncludeDirectories(BuildSystem.ProcessDirectory);
			profile.AddIncludeDirectories(sourcePathRoot);
			profile.AddIncludeDirectories(generatedFilesPath);
			if (rules.DependencyModulesName != null)
				foreach (string dep in rules.DependencyModulesName)
				{
					if (!BuildSystem.SourceBuilders.ContainsKey(dep))
						continue;

					SourceBuilder builder = BuildSystem.SourceBuilders[dep];

					profile.AddIncludeDirectories(builder.sourcePathRoot);

					if (builder.rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] libFiles = builder.rules.OutputsFilePath;

						if (libFiles != null)
							foreach (string libFile in libFiles)
								profile.AddIncludeLibraries(builder.sourcePathRoot + libFile);
					}
					else
					{
						profile.AddPreprocessorDefinition(GetAPIPreprocessor(builder.rules, true));

						string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + LibExtension);

						if (libFiles != null)
							foreach (string libFile in libFiles)
								profile.AddIncludeLibraries(libFile);
					}
				}

			profile.AddPreprocessorDefinition(GetAPIPreprocessor(rules, false));
			if (rules.PreprocessorDefinitions != null)
				foreach (string def in rules.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			if (rules.DependencyStaticLibraries != null)
				foreach (string lib in rules.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			bool isThisCommonModule = (rules.TargetName == "Common");

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.h*");
			foreach (string file in files)
			{
				if (BuildSystem.GenerateReflection && !isThisCommonModule)
				{
					string outputBaseFileName = generatedFilesPath + Path.GetFileNameWithoutExtension(file) + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + ".h");
						cppProj.AddCompileFile(outputBaseFileName + ".cpp");
					}
				}

				cppProj.AddIncludeFile(file);
			}

			if (rules.IncludeModulesName != null)
				foreach (string moduleName in rules.IncludeModulesName)
				{
					SourceBuilder builder = BuildSystem.SourceBuilders[moduleName];

					profile.AddIncludeDirectories(builder.sourcePathRoot);
					profile.AddPreprocessorDefinition(GetEmptyAPIPreprocessor(builder.rules));
				}

			if (rules.AdditionalCompileFile != null)
				foreach (string file in rules.AdditionalCompileFile)
				{
					string fileInfo = FileSystemUtilites.PathSeperatorCorrection(file);
					int index = fileInfo.IndexOf(EnvironmentHelper.PathSeparator);

					cppProj.AddCompileFile(BuildSystem.SourceBuilders[fileInfo.Remove(index)].sourcePathRoot + fileInfo.Substring(index + 1));
				}

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.c", "*.cpp", "*.cxx");
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			string vcprojPath = intermediateModulePath + rules.TargetName + ".vcxproj";

			MicrosoftVCProjectGenerator vcProjectGenerator = new MicrosoftVCProjectGenerator();
			vcProjectGenerator.ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;

			File.WriteAllText(vcprojPath, vcProjectGenerator.Generate(cppProj));

			BuildProjectFile(vcprojPath);
		}

		private void BuildProjectFile()
		{
			LogInfo();

			BuildProjectFile(sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(rules.ProjectFilePath));
		}

		private void BuildProjectFile(string ProjectFile)
		{
			if (compiler.BuildProjectFile(ProjectFile))
			{
				if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, ExeExtension);
				else if (rules.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, DLLExtension);

				state = States.Built;

				return;
			}

			ConsoleHelper.WriteLineError("Building " + rules.TargetName + " failed");

			state = States.Failed;
		}

		private static void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, BuildSystem.FinalOutputDirectory, true, "*" + Extension);
		}

		private void ErrorRaised(string Text)
		{
			LogHelper.WriteLineError(rules.TargetName, Text);
		}

		private bool ParseForReflection(string FilePath, string OutputBaseFileName)
		{
			if (FilePath.EndsWith("ReflectionDefinitions.h"))
				return false;

			if (reflectionGeneratorProcess == null)
			{
				if (!File.Exists(BuildSystem.ReflectionToolPath))
					return false;

				reflectionGeneratorProcess = new CommandLineProcess(BuildSystem.ReflectionToolPath);
			}

			reflectionGeneratorProcess.Start("\"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			while (!reflectionGeneratorProcess.Output.EndOfStream)
				reflectionGeneratorProcess.Output.ReadLine();

			return (reflectionGeneratorProcess.ExitCode == 0);
		}

		private static ProjectBase.ProfileBase.OutputTypes LibraryUseTypesToOutputType(BuildRules.LibraryUseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case BuildRules.LibraryUseTypes.Executable:
					return ProjectBase.ProfileBase.OutputTypes.Application;

				case BuildRules.LibraryUseTypes.DynamicLibrary:
					return ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;

				case BuildRules.LibraryUseTypes.StaticLibrary:
					return ProjectBase.ProfileBase.OutputTypes.StaticLinkLibrary;

				default:
					throw new Exception(LibraryUseType + " cannot cast to OutputTypes");
			}
		}

		private static string GetEmptyAPIPreprocessor(BuildRules Rules)
		{
			return Rules.TargetName.ToUpper() + "_API=";
		}

		private static string GetAPIPreprocessor(BuildRules Rules, bool IsImport)
		{
			return GetEmptyAPIPreprocessor(Rules) + "__declspec(dll" + (IsImport ? "import" : "export") + ")";
		}
	}
}
