// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System;
using System.IO;
using System.Reflection;

namespace Engine.Frontend
{
	public static class EnvironmentHelper
	{
		public enum Platforms
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
		public const string ReflectionModuleName = "Reflection";
		public const string GeneratedPathName = "Generated";
		public const string ReflectionToolName = "ReflectionTool";

		private static Type monoRuntime = Type.GetType("Mono.Runtime");

		public static readonly string HeaderFileExtension = ".h";
		public static readonly string CompileFileExtension = ".cpp";
		public static readonly string CSharpFileExtension = ".cs";

		public static readonly string[] HeaderFileExtensions = new string[] { "*.h", "*.hpp" };
		public static readonly string[] CompileFileExtensions = new string[] { "*.cpp", "*.c", "*.cxx" };
		public static readonly string[] CSharpFileExtensions = new string[] { "*.cs" };

		public static Platforms Platform
		{
			get { return (Environment.OSVersion.Platform == PlatformID.Win32NT ? Platforms.Windows : Platforms.Linux); }
		}

		public static ManagedRuntimes ManagedRuntime
		{
			get { return (monoRuntime == null ? ManagedRuntimes.DotNet : ManagedRuntimes.Mono); }
		}

		public static string ExecutableExtentions
		{
			get { return (Platform == Platforms.Windows ? ".exe" : ".aaa"); }
		}

		public static string DynamicLibraryExtentions
		{
			get { return (Platform == Platforms.Windows ? ".dll" : ".bbb"); }
		}

		public static string StaticLibraryExtentions
		{
			get { return (Platform == Platforms.Windows ? ".lib" : ".ccc"); }
		}

		public static char PathSeparator
		{
			get { return (Platform == Platforms.Windows ? '\\' : '/'); }
		}

		public static string BinariesDirectory
		{
			get { return Path.GetFullPath(FrontenddToolDirectory + ".." + PathSeparator + ".." + PathSeparator + BinariesPathName + PathSeparator); }
		}

		public static string OutputPathName
		{
			get { return BuildSystemHelper.GetOutputPathName(BuildSystemHelper.BuildConfiguration, BuildSystemHelper.PlatformArchitecture); }
		}

		public static string OutputDirectory
		{
			get { return BuildSystemHelper.GetOutputDirectory(BuildSystemHelper.BuildConfiguration, BuildSystemHelper.PlatformArchitecture); }
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

		public static string FrontenddToolPath
		{
			get { return Assembly.GetExecutingAssembly().Location; }
		}

		private static string FrontenddToolDirectory
		{
			get { return Path.GetDirectoryName(FrontenddToolPath) + PathSeparator; }
		}

		public static string ReflectionToolPath
		{
			get { return OutputDirectory + ReflectionToolName + ExecutableExtentions; }
		}
	}
}