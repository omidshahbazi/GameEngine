// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend.ProjectFileGenerator
{
	//https://msdn.microsoft.com/en-us/library/dd293607.aspx

	class VCProjectFile : MSBuildProjectFile
	{
		public enum ToolsVersions
		{
			v9_0 = 0,
			v10_0,
			v11_0,
			v12_0,
			v14_0
		}

		public enum PlatformTypes
		{
			Win32 = 0,
			x64
		}

		public enum RuntimeLibraries
		{
			MultiThreaded = 0,
			MultiThreadedDebug,
			MultiThreadedDLL,
			MultiThreadedDebugDLL
		}

		public enum Optimizations
		{
			Disabled = 0
		}

		private StringList includeFiles = new StringList();
		private StringList additionalLibraryDirectories = new StringList();
		private StringList includeDirectories = new StringList();
		private StringList includeLibraryDirectories = new StringList();
		private StringList includeLibraries = new StringList();

		public override string Content
		{
			get
			{
				XmlElement projectElement = ProjectElement;

				XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
				XmlElement projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
				XmlElement configuration = CreateElement("Configuration", projectConfiguration);
				XmlElement platform = CreateElement("Platform", projectConfiguration);

				XmlElement import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.default.props");

				XmlElement popertyGroup = CreateElement("PropertyGroup", projectElement);
				XmlElement configurationType = CreateElement("ConfigurationType", popertyGroup);
				XmlElement platformToolset = CreateElement("PlatformToolset", popertyGroup);

				popertyGroup = CreateElement("PropertyGroup", projectElement);
				XmlElement outDir = CreateElement("OutDir", popertyGroup);
				XmlElement targetName = CreateElement("TargetName", popertyGroup);

				XmlElement clCompile = CreateElement("ClCompile", CreateElement("ItemDefinitionGroup", projectElement));
				XmlElement runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

				XmlElement itemDefinitionGroup = CreateElement("ItemDefinitionGroup", projectElement);
				clCompile = CreateElement("ClCompile", itemDefinitionGroup);
				XmlElement additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
				XmlElement preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
				XmlElement optimization = CreateElement("Optimization", clCompile);
				XmlElement minimalRebuild = CreateElement("MinimalRebuild", clCompile);
				XmlElement link = CreateElement("Link", itemDefinitionGroup);
				XmlElement generateDebugInformation = CreateElement("GenerateDebugInformation", link);
				XmlElement additionalLibraryDirectories = CreateElement("AdditionalLibraryDirectories", link);
				XmlElement additionalLibraries = CreateElement("AdditionalDependencies", link);

				XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
				XmlElement compileFiles = CreateElement("ItemGroup", projectElement);

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.Targets");

				return projectElement.OwnerDocument.OuterXml;
			}
		}

		public string[] IncludeFiles
		{
			get { return includeFiles.ToArray(); }
		}

		public string[] AdditionalLibraryDirectories
		{
			get { return additionalLibraryDirectories.ToArray(); }
		}

		public string[] IncludeDirectories
		{
			get { return includeDirectories.ToArray(); }
		}

		public string[] IncludeLibraryDirectories
		{
			get { return includeLibraryDirectories.ToArray(); }
		}

		public string[] IncludeLibraries
		{
			get { return includeLibraries.ToArray(); }
		}

		public ToolsVersions ToolsVersion
		{
			get;
			set;
		}

		public BuildConfigurations BuildConfiguration
		{
			get;
			set;
		}

		public PlatformTypes PlatformType
		{
			get;
			set;
		}

		public RuntimeLibraries RuntimeLibrary
		{
			get;
			set;
		}

		public bool GenerateDebugInformation
		{
			get;
			set;
		}

		public Optimizations Optimization
		{
			get;
			set;
		}

		public bool MinimalRebuild
		{
			get;
			set;
		}

		public void AddIncludeFile(string FilePath)
		{
			includeFiles.Add(FilePath);
		}

		public void AddAdditionalLibraryDirectories(string FilePath)
		{
			additionalLibraryDirectories.Add(FilePath);
		}

		public void AddIncludeDirectories(string FilePath)
		{
			includeDirectories.Add(FilePath);
		}

		public void AddIncludeLibraryDirectories(string FilePath)
		{
			includeLibraryDirectories.Add(FilePath);
		}

		public void AddIncludeLibraries(string FilePath)
		{
			includeLibraries.Add(FilePath);
		}

		private string GetOutputType()
		{
			string type = "";

			switch (OutputType)
			{
				case OutputTypes.Application:
					type = "Exe";
					break;
				case OutputTypes.DynamicLinkLibrary:
					type = "Library";
					break;
				case OutputTypes.StaticLinkLibrary:
					type = "StaticLibrary";
					break;
			}

			return type;
		}
	}
}
