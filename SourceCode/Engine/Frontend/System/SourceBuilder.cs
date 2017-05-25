// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.ProjectFileGenerator;
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

			VCProjectFile vcproj = new VCProjectFile();
			vcproj.AssemblyName = rules.TargetName;
			vcproj.OutputType = LibraryUseTypesToOutputType(rules.LibraryUseType);
			vcproj.OutputPath = BinariesPath;
			vcproj.Optimization = VCProjectFile.Optimizations.Disabled;
			vcproj.MinimalRebuild = true;
			vcproj.PlatformType = BuildSystem.PlatformType;
			vcproj.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (vcproj.BuildConfiguration == VCProjectFile.BuildConfigurations.Debug)
			{
				vcproj.GenerateDebugInformation = true;

				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDebug;
				//else
				vcproj.RuntimeLibrary = VCProjectFile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else
				vcproj.GenerateDebugInformation = false;
			{
				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreaded;
				//else
				vcproj.RuntimeLibrary = VCProjectFile.RuntimeLibraries.MultiThreadedDLL;
			}

			vcproj.AddIncludeDirectories(BuildSystem.ProcessDirectory);
			vcproj.AddIncludeDirectories(sourcePathRoot);
			vcproj.AddIncludeDirectories(generatedFilesPath);
			if (rules.DependencyModulesName != null)
				foreach (string dep in rules.DependencyModulesName)
				{
					if (!BuildSystem.SourceBuilders.ContainsKey(dep))
						continue;

					SourceBuilder builder = BuildSystem.SourceBuilders[dep];

					vcproj.AddIncludeDirectories(builder.sourcePathRoot);

					if (builder.rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] libFiles = builder.rules.OutputsFilePath;

						if (libFiles != null)
							foreach (string libFile in libFiles)
								vcproj.AddIncludeLibraries(builder.sourcePathRoot + libFile);
					}
					else
					{
						vcproj.AddPreprocessorDefinition(GetAPIPreprocessor(builder.rules, true));

						string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + LibExtension);

						if (libFiles != null)
							foreach (string libFile in libFiles)
								vcproj.AddIncludeLibraries(libFile);
					}
				}

			vcproj.AddPreprocessorDefinition(GetAPIPreprocessor(rules, false));
			if (rules.PreprocessorDefinitions != null)
				foreach (string def in rules.PreprocessorDefinitions)
					vcproj.AddPreprocessorDefinition(def);

			if (rules.DependencyStaticLibraries != null)
				foreach (string lib in rules.DependencyStaticLibraries)
					vcproj.AddIncludeLibraries(lib);

			bool isThisCommonModule = (rules.TargetName == "Common");

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.h*");
			foreach (string file in files)
			{
				if (BuildSystem.GenerateReflection && !isThisCommonModule)
				{
					string outputBaseFileName = generatedFilesPath + Path.GetFileNameWithoutExtension(file) + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						vcproj.AddIncludeFile(outputBaseFileName + ".h");
						vcproj.AddCompileFile(outputBaseFileName + ".cpp");
					}
				}

				vcproj.AddIncludeFile(file);
			}

			if (rules.IncludeModulesName != null)
				foreach (string moduleName in rules.IncludeModulesName)
				{
					SourceBuilder builder = BuildSystem.SourceBuilders[moduleName];

					vcproj.AddIncludeDirectories(builder.sourcePathRoot);
					vcproj.AddPreprocessorDefinition(GetEmptyAPIPreprocessor(builder.rules));
				}

			if (rules.AdditionalCompileFile != null)
				foreach (string file in rules.AdditionalCompileFile)
				{
					string fileInfo = FileSystemUtilites.PathSeperatorCorrection (file);
					int index = fileInfo.IndexOf(EnvironmentHelper.PathSeparator);

					vcproj.AddCompileFile(BuildSystem.SourceBuilders[fileInfo.Remove(index)].sourcePathRoot + fileInfo.Substring(index + 1));
				}

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.c", "*.cpp", "*.cxx");
			foreach (string file in files)
				vcproj.AddCompileFile(file);

			string vcprojPath = intermediateModulePath + rules.TargetName + ".vcxproj";

			File.WriteAllText(vcprojPath, vcproj.Content);

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

		private static ProjectFile.OutputTypes LibraryUseTypesToOutputType(BuildRules.LibraryUseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case BuildRules.LibraryUseTypes.Executable:
					return ProjectFile.OutputTypes.Application;

				case BuildRules.LibraryUseTypes.DynamicLibrary:
					return ProjectFile.OutputTypes.DynamicLinkLibrary;

				case BuildRules.LibraryUseTypes.StaticLibrary:
					return ProjectFile.OutputTypes.StaticLinkLibrary;

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
