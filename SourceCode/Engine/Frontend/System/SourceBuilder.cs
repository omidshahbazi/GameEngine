// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.IO;

namespace Engine.Frontend
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

			VCProjectFileGenerator vcproj = new VCProjectFileGenerator();
			vcproj.AssemblyName = rules.TargetName;
			vcproj.OutputType = LibraryUseTypesToOutputType(rules.LibraryUseType);
			vcproj.OutputPath = BinariesPath;
			vcproj.Optimization = VCProjectFileGenerator.Optimizations.Disabled;
			vcproj.MinimalRebuild = true;
			vcproj.PlatformType = BuildSystem.PlatformType;
			vcproj.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (vcproj.BuildConfiguration == VCProjectFileGenerator.BuildConfigurations.Debug)
			{
				vcproj.GenerateDebugInformation = true;

				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDebug;
				//else
				vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else
				vcproj.GenerateDebugInformation = false;
			{
				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreaded;
				//else
				vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDLL;
			}

			vcproj.IncludeDirectories += BuildSystem.ProcessDirectory;
			vcproj.IncludeDirectories += sourcePathRoot;
			vcproj.IncludeDirectories += generatedFilesPath;
			if (rules.DependencyModulesName != null)
				foreach (string dep in rules.DependencyModulesName)
				{
					if (!BuildSystem.SourceBuilders.ContainsKey(dep))
						continue;

					SourceBuilder builder = BuildSystem.SourceBuilders[dep];

					vcproj.IncludeDirectories += builder.sourcePathRoot;

					if (builder.rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] libFiles = builder.rules.OutputsFilePath;

						if (libFiles != null)
							foreach (string libFile in libFiles)
								vcproj.IncludeLibraries += builder.sourcePathRoot + libFile;
					}
					else
					{
						vcproj.PreprocessorDefinitions += GetAPIPreprocessor(builder.rules, true);

						string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + LibExtension);

						if (libFiles != null)
							foreach (string libFile in libFiles)
								vcproj.IncludeLibraries += libFile;
					}
				}

			vcproj.PreprocessorDefinitions += GetAPIPreprocessor(rules, false);
			if (rules.PreprocessorDefinitions != null)
				foreach (string def in rules.PreprocessorDefinitions)
					vcproj.PreprocessorDefinitions += def;

			if (rules.DependencyStaticLibraries != null)
				foreach (string lib in rules.DependencyStaticLibraries)
					vcproj.IncludeLibraries += lib;

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

					vcproj.IncludeDirectories += builder.sourcePathRoot;
					vcproj.PreprocessorDefinitions += GetEmptyAPIPreprocessor(builder.rules);
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

			vcproj.Generate(vcprojPath);

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

		private static ProjectFileGenerator.OutputTypes LibraryUseTypesToOutputType(BuildRules.LibraryUseTypes LibraryUseType)
		{
			switch (LibraryUseType)
			{
				case BuildRules.LibraryUseTypes.Executable:
					return ProjectFileGenerator.OutputTypes.Application;

				case BuildRules.LibraryUseTypes.DynamicLibrary:
					return ProjectFileGenerator.OutputTypes.DynamicLinkLibrary;

				case BuildRules.LibraryUseTypes.StaticLibrary:
					return ProjectFileGenerator.OutputTypes.StaticLinkLibrary;

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
