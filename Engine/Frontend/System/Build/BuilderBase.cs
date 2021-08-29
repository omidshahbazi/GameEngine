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

		protected string IntermediateModulePath
		{
			get { return EnvironmentHelper.IntermediateDirectory + ModuleName + EnvironmentHelper.PathSeparator; }
		}

		protected string GeneratedFilesPath
		{
			get { return IntermediateModulePath +EnvironmentHelper.GeneratedPathName + EnvironmentHelper.PathSeparator; }
		}

		protected string IntermediateOutputPaths
		{
			get { return IntermediateModulePath + EnvironmentHelper.BinariesPathName + EnvironmentHelper.PathSeparator + EnvironmentHelper.OutputPathName; }
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

		public abstract bool Build(bool ForceToRebuild);

		protected bool Compile(string ProjectFile)
		{
			return Compiler.Build(ProjectFile, BuildSystemHelper.BuildConfiguration, BuildSystemHelper.PlatformArchitecture);
		}

		protected bool Compile(ProjectBase.ProfileBase ProjectProfile)
		{
			return Compiler.Build(ProjectProfile);
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
