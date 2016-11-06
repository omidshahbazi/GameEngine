// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;

namespace Engine.Frontend
{
	public static class EnvironmentHelper
	{
		public enum Platforms
		{
			Windows = 0,
			Linux
		}

		public enum Runtimes
		{
			DotNet = 0,
			Mono
		}

		private static Type monoRuntime = Type.GetType("Mono.Runtime");

		public static Platforms Platform
		{
			get { return (Environment.OSVersion.Platform == PlatformID.Win32NT ? Platforms.Windows : Platforms.Linux); }
		}

		public static Runtimes Runtime
		{
			get { return (monoRuntime == null ? Runtimes.DotNet : Runtimes.Mono); }
		}

		public static char PathSeparator
		{
			get { return (Platform == Platforms.Windows ? '\\' : '/'); }
		}
	}
}