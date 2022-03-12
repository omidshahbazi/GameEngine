// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;
using System;
using System.IO;
using System.Reflection;

namespace Engine.Frontend.System
{
	static class EnvironmentHelper
	{
		public enum OperatingSystems
		{
			Windows = 0,
			Linux
		}

		public enum ManagedRuntimes
		{
			DotNet = 0,
			Mono
		}

		public const string BinariesPathName = "Binaries";
		public const string GeneratedPathName = "Generated";
		public const string ReflectionModuleName = "Reflection";
		public const string ReflectionToolModuleName = "ReflectionTool";
		public const string ReflectionToolFileName = "ReflectionTool";

		private static Type monoRuntime = Type.GetType("Mono.Runtime");

		public static readonly string HeaderFileExtension = ".h";
		public static readonly string CompileFileExtension = ".cpp";
		public static readonly string CSharpFileExtension = ".cs";

		public static readonly string[] HeaderFileExtensions = new string[] { "*.h", "*.hpp" };
		public static readonly string[] CompileFileExtensions = new string[] { "*.cpp", "*.c", "*.cxx" };
		public static readonly string[] CSharpFileExtensions = new string[] { "*.cs" };
		public static readonly string[] DebugVisualizerFileExtensions = new string[] { "*.natvis" };

		private static string FrontenddToolDirectory
		{
			get { return Path.GetDirectoryName(FrontenddToolPath) + PathSeparator; }
		}

		public static OperatingSystems OperatingSystem
		{
			get { return (Environment.OSVersion.Platform == PlatformID.Win32NT ? OperatingSystems.Windows : OperatingSystems.Linux); }
		}

		public static ManagedRuntimes ManagedRuntime
		{
			get { return (monoRuntime == null ? ManagedRuntimes.DotNet : ManagedRuntimes.Mono); }
		}

		public static string ExecutableExtentions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".exe" : ".aaa"); }
		}

		public static string DynamicLibraryExtentions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".dll" : ".bbb"); }
		}

		public static string StaticLibraryExtentions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".lib" : ".ccc"); }
		}

		public static char PathSeparator
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? '\\' : '/'); }
		}

		public static string FrontenddToolPath
		{
			get { return Assembly.GetExecutingAssembly().Location; }
		}

		public static string BinariesDirectory
		{
			get;
			set;
		}

		public static string RooDirectory
		{
			get { return Path.GetFullPath(BinariesDirectory + ".." + PathSeparator + ".." + PathSeparator); }
		}

		public static string EngineDirectory
		{
			get { return RooDirectory + "Engine" + PathSeparator; }
		}

		public static string SourceDirectory
		{
			get { return EngineDirectory + "Source" + PathSeparator; }
		}

		public static string IntermediateDirectory
		{
			get { return EngineDirectory + "Intermediate" + PathSeparator; }
		}

		static EnvironmentHelper()
		{
			BinariesDirectory = Path.GetFullPath(FrontenddToolDirectory + ".." + PathSeparator + ".." + PathSeparator + BinariesPathName + PathSeparator);
		}

		public static string GetReflectionToolPath(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return GetOutputDirectory(Configuration, Architecture) + ReflectionToolFileName + ExecutableExtentions;
		}

		public static string GetOutputPathName(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return $"{Configuration} {Architecture.GetPrettyName()}" + PathSeparator;
		}

		public static string GetOutputDirectory(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return BinariesDirectory + GetOutputPathName(Configuration, Architecture);
		}
	}
}