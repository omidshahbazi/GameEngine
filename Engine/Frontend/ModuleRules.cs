// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System.Collections.Generic;

namespace Engine
{
	public abstract class ModuleRules : BaseRules
	{
		public enum UseTypes
		{
			Executable = 0,
			DynamicLibrary,
			StaticLibrary,
			UseOnly
		}

		internal const string FilePostfix = ".ModuleRules.cs";

		public abstract string Name
		{
			get;
		}

		public abstract UseTypes UseType
		{
			get;
		}

		public string TargetName
		{
			get;
			set;
		}

		public List<string> PreprocessorDefinitions
		{
			get;
			private set;
		}

		public List<string> PrivateDependencyModuleNames
		{
			get;
			private set;
		}

		public List<string> PublicDependencyModuleNames
		{
			get;
			private set;
		}

		public List<string> DependencyStaticLibraries
		{
			get;
			private set;
		}

		public List<string> IncludePaths
		{
			get;
			private set;
		}

		public List<string> BinaryPaths
		{
			get;
			private set;
		}

		public List<string> LibraryPaths
		{
			get;
			private set;
		}

		public bool GenerateReflection
		{
			get;
			set;
		}

		public bool GenerateRenderDocSettings
		{
			get;
			set;
		}

		public ModuleRules(Configurations Configuration, Platforms Platform)
		{
			TargetName = Name;

			PreprocessorDefinitions = new List<string>();
			PrivateDependencyModuleNames = new List<string>();
			PublicDependencyModuleNames = new List<string>();
			DependencyStaticLibraries = new List<string>();
			IncludePaths = new List<string>();
			BinaryPaths = new List<string>();
			LibraryPaths = new List<string>();
			GenerateReflection = false;
		}
	}
}