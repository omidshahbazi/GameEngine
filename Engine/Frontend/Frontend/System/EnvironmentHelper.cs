// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;
using GameFramework.ASCIISerializer;
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
		public const string InfoFileName = "Info.json";

		private static Type monoRuntime = Type.GetType("Mono.Runtime");

		public const string HeaderFileExtension = ".h";
		public const string CompileFileExtension = ".cpp";

		public static readonly string[] HeaderFileSearchPattern = new string[] { "*.h", "*.hpp" };
		public static readonly string[] CompileFileSearchPattern = new string[] { "*.cpp", "*.c", "*.cxx" };
		public static readonly string[] CSharpFileSearchPattern = new string[] { "*.cs" };
		public static readonly string[] DebugVisualizerFileSearchPattern = new string[] { "*.natvis" };

		public const string DebugModeProjectPreprocessor = "DEBUG_MODE_PROJECT";

		private static string FrontenddToolDirectory
		{
			get { return Path.GetDirectoryName(FrontenddToolPath) + PathSeparator; }
		}

		public static Version Version
		{
			get;
			private set;
		}

		public static string Copyright
		{
			get;
			private set;
		}

		public static OperatingSystems OperatingSystem
		{
			get { return (Environment.OSVersion.Platform == PlatformID.Win32NT ? OperatingSystems.Windows : OperatingSystems.Linux); }
		}

		public static ManagedRuntimes ManagedRuntime
		{
			get { return (monoRuntime == null ? ManagedRuntimes.DotNet : ManagedRuntimes.Mono); }
		}

		public static string ExecutableExtensions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".exe" : ".aaa"); }
		}

		public static string DynamicLibraryExtensions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".dll" : ".bbb"); }
		}

		public static string StaticLibraryExtensions
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".lib" : ".ccc"); }
		}

		public static string IconExtension
		{
			get { return (OperatingSystem == OperatingSystems.Windows ? ".ico" : ".ccc"); }
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
			get { return Path.GetFullPath($"{FrontenddToolDirectory}..{PathSeparator}..{PathSeparator}{BinariesPathName}{PathSeparator}"); }
		}

		public static string EngineRooDirectory
		{
			get { return Path.GetFullPath($"{BinariesDirectory}..{PathSeparator}..{PathSeparator}"); }
		}

		public static string EngineDirectory
		{
			get { return $"{EngineRooDirectory}Engine{PathSeparator}"; }
		}

		public static string SourceDirectory
		{
			get { return $"{EngineDirectory}Source{PathSeparator}"; }
		}

		public static string ContentsDirectory
		{
			get { return $"{EngineDirectory}Contents{PathSeparator}"; }
		}

		public static string IntermediateDirectory
		{
			get { return $"{EngineDirectory}Intermediate{PathSeparator}"; }
		}

		//In the future, we need to override this based on where it should working on
		public static string WorkingDirectory
		{
			get;
			set;
		}

		static EnvironmentHelper()
		{
			WorkingDirectory = EngineRooDirectory;

			ISerializeObject obj = Creator.Create<ISerializeObject>(File.ReadAllText(EngineDirectory + InfoFileName));

			Version = new Version(obj.Get<string>("Version", "0.0.0.0"));
			Copyright = obj.Get<string>("Copyright");
		}

		public static string GetReflectionToolPath(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return GetOutputDirectory(Configuration, Architecture) + ReflectionToolFileName + ExecutableExtensions;
		}

		public static string GetOutputPathName(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return $"{Configuration} {Architecture.GetPrettyName()}{PathSeparator}";
		}

		public static string GetOutputDirectory(ProjectBase.ProfileBase.BuildConfigurations Configuration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			return BinariesDirectory + GetOutputPathName(Configuration, Architecture);
		}
	}
}