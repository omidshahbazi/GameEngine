// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.IO;
using System.Reflection;

namespace Engine.Frontend.System
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

		private static Type monoRuntime = Type.GetType("Mono.Runtime");

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

		public static string ExecutingPath
		{
			get { return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + EnvironmentHelper.PathSeparator; }
		}

		public static string FinalOutputDirectory
		{
			get;
			private set;
		}

		public static string RooDirectory
		{
			get;
			private set;
		}

		public static string IntermediateDirectory
		{
			get;
			private set;
		}

		public static void Initialize()
		{
			FinalOutputDirectory = ExecutingPath;
			RooDirectory = Path.GetFullPath(FinalOutputDirectory + ".." + PathSeparator);
			IntermediateDirectory = RooDirectory + "Intermediate" + PathSeparator;
		}
	}
}