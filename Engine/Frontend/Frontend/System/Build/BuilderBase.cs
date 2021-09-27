// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;

namespace Engine.Frontend.System.Build
{
	abstract class BuilderBase
	{
		public enum States
		{
			NotBuilt = 0,
			Built,
			AlreadyUpdated,
			Failed
		}

		protected abstract string ModuleName
		{
			get;
		}

		protected string IntermediateModuleDirectory
		{
			get { return EnvironmentHelper.IntermediateDirectory + ModuleName + EnvironmentHelper.PathSeparator; }
		}

		protected string IntermediateGeneratedDirectory
		{
			get { return IntermediateModuleDirectory + EnvironmentHelper.GeneratedPathName + EnvironmentHelper.PathSeparator; }
		}

		protected string IntermediateTempPath
		{
			get { return IntermediateModuleDirectory + EnvironmentHelper.OutputPathName; }
		}

		protected string IntermediateOutputPath
		{
			get { return IntermediateTempPath + EnvironmentHelper.BinariesPathName + EnvironmentHelper.PathSeparator; }
		}

		public States State
		{
			get;
			protected set;
		}

		protected Compiler Compiler
		{
			get;
			private set;
		}

		public BuilderBase()
		{
			Compiler = new Compiler();
			Compiler.ErrorRaised += ErrorRaised;
		}

		public void Initialize()
		{
			CreateDirectories();
		}

		public abstract void Build(bool ForceToRebuild);

		protected void Compile(string ProjectFile)
		{
			Compiler.Build(ProjectFile, BuildSystemHelper.BuildConfiguration, BuildSystemHelper.PlatformArchitecture);
		}

		protected void Compile(ProjectBase.ProfileBase ProjectProfile)
		{
			Compiler.Build(ProjectProfile);
		}

		protected void LogCurrentInfo()
		{
			LogHelper.DeleteLog(ModuleName);
			ConsoleHelper.WriteInfo("	Building " + ModuleName);
		}

		protected static void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, EnvironmentHelper.OutputDirectory, true, "*" + Extension);
		}

		protected static int GetHash(string Value)
		{
			return Value.GetHashCode();
		}

		protected virtual void CreateDirectories()
		{
		}

		private void ErrorRaised(string Text)
		{
			LogHelper.WriteLineError(ModuleName, Text);
			ConsoleHelper.WriteError(Text.Replace("{", "{{").Replace("}", "}}"));
		}
	}
}
