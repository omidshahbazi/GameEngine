// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;

namespace Engine.Frontend.System.Build
{
	abstract class BaseBuilder
	{
		public enum States
		{
			NotBuilt = 0,
			Built,
			AlreadyUpdated,
			Failed
		}

		private Compiler compiler = null;

		public abstract string ModuleName
		{
			get;
		}

		protected ProjectBase.ProfileBase.BuildConfigurations Configuration
		{
			get;
			private set;
		}

		protected ProjectBase.ProfileBase.PlatformArchitectures Architecture
		{
			get;
			private set;
		}

		protected string IntermediateModuleDirectory
		{
			get { return ModuleName.GetIntermediateDirectory(); }
		}

		protected string IntermediateGeneratedDirectory
		{
			get { return ModuleName.GetIntermediateGeneratedDirectory(); }
		}

		protected string IntermediateTempPath
		{
			get { return ModuleName.GetIntermediateTempPath(Configuration, Architecture); }
		}

		protected string IntermediateOutputPath
		{
			get { return ModuleName.GetIntermediateOutputPath(Configuration, Architecture); }
		}

		protected string FinalOutputDirectory
		{
			get { return EnvironmentHelper.GetOutputDirectory(Configuration, Architecture); }
		}

		public States State
		{
			get;
			protected set;
		}

		public BaseBuilder(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			compiler = new Compiler();
			compiler.ErrorRaised += ErrorRaised;

			this.Configuration = Configuration;
			this.Architecture = Architecture;
		}

		public abstract void Build(bool ForceToRebuild);

		protected void Initialize()
		{
			CreateDirectories();
		}

		protected void Compile(string ProjectFile)
		{
			compiler.Build(ProjectFile, Configuration, Architecture);
		}

		protected void Compile(ProjectBase.ProfileBase ProjectProfile)
		{
			compiler.Build(ProjectProfile);
		}

		protected void LogCurrentInfo()
		{
			LogHelper.DeleteLog(ModuleName);
			ConsoleHelper.WriteInfo("	Building " + ModuleName);
		}

		protected void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, FinalOutputDirectory, true, "*" + Extension);
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

		//public static
	}
}
