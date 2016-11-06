// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend
{
	//https://msdn.microsoft.com/en-us/library/dd293607.aspx

	class VCProjectFileGenerator : MSBuildProjectFileGenerator
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
			Win64
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

		private BuildConfigurations buildConfiguration = BuildConfigurations.Debug;
		private PlatformTypes platformType = PlatformTypes.Win32;

		private XmlElement projectConfiguration = null;
		private XmlElement configuration = null;
		private XmlElement platform = null;
		private XmlElement configurationType = null;
		private XmlElement platformToolset = null;
		private XmlElement outDir = null;
		private XmlElement targetName = null;
		private XmlElement runtimeLibrary = null;
		private XmlElement additionalIncludeDirectories = null;
		private XmlElement preprocessorDefinitions = null;
		private XmlElement optimization = null;
		private XmlElement minimalRebuild = null;
		private XmlElement link = null;
		private XmlElement generateDebugInformation = null;
		private XmlElement additionalLibraryDirectories = null;
		private XmlElement additionalLibraries = null;
		private XmlElement includeFiles = null;
		private XmlElement compileFiles = null;

		public override string AssemblyName
		{
			get { return targetName.InnerText; }
			set { targetName.InnerText = value; }
		}

		public override string OutputPath
		{
			get { return outDir.InnerText; }
			set { outDir.InnerText = value; }
		}

		public override OutputTypes OutputType
		{
			get
			{
				string type = configurationType.InnerText;

				if (type == "DynamicLibrary")
					return OutputTypes.DynamicLinkLibrary;
				else if (type == "StaticLibrary")
					return OutputTypes.StaticLinkLibrary;
				else
					return OutputTypes.Application;
			}
			set
			{
				string type = "";

				if (value == OutputTypes.Application)
					type = "Application";
				else if (value == OutputTypes.DynamicLinkLibrary)
					type = "DynamicLibrary";
				else if (value == OutputTypes.StaticLinkLibrary)
					type = "StaticLibrary";

				configurationType.InnerText = type;
			}
		}

		public override string PreprocessorDefinitions
		{
			get { return preprocessorDefinitions.InnerText; }
			set { preprocessorDefinitions.InnerText = value + (value.EndsWith(";") ? string.Empty : ";"); }
		}

		public string AdditionalLibraryDirectories
		{
			get { return additionalLibraryDirectories.InnerText; }
			set { additionalLibraryDirectories.InnerText = value + (value.EndsWith(";") ? string.Empty : ";"); }
		}

		public string IncludeDirectories
		{
			get { return additionalIncludeDirectories.InnerText; }
			set { additionalIncludeDirectories.InnerText = value + (value.EndsWith(";") ? string.Empty : ";"); }
		}

		public string IncludeLibraryDirectories
		{
			get { return additionalLibraryDirectories.InnerText; }
			set { additionalLibraryDirectories.InnerText = value + (value.EndsWith(";") ? string.Empty : ";"); }
		}

		public string IncludeLibraries
		{
			get { return additionalLibraries.InnerText; }
			set { additionalLibraries.InnerText = value + (value.EndsWith(";") ? string.Empty : ";"); }
		}

		public ToolsVersions ToolsVersion
		{
			get
			{
				string version = Project.GetAttribute("ToolsVersion");

				if (string.IsNullOrEmpty(version))
					return ToolsVersions.v12_0;

				return (ToolsVersions)Enum.Parse(typeof(ToolsVersions), "v" + version.Replace('.', '_'));
			}
			set
			{
				Project.SetAttribute("ToolsVersion", value.ToString().Substring(1).Replace('_', '.'));
				platformToolset.InnerText = value.ToString().Replace("_", "");
			}
		}

		public BuildConfigurations BuildConfiguration
		{
			get { return buildConfiguration; }
			set
			{
				buildConfiguration = value;

				configuration.InnerText = buildConfiguration.ToString();

				projectConfiguration.SetAttribute("Include", buildConfiguration.ToString() + "|" + platformType.ToString());
			}
		}

		public PlatformTypes PlatformType
		{
			get { return platformType; }
			set
			{
				platformType = value;

				platform.InnerText = platformType.ToString();


				projectConfiguration.SetAttribute("Include", buildConfiguration.ToString() + "|" + platformType.ToString());
			}
		}

		public RuntimeLibraries RuntimeLibrary
		{
			get { return (RuntimeLibraries)Enum.Parse(typeof(RuntimeLibraries), runtimeLibrary.InnerText); }
			set { runtimeLibrary.InnerText = value.ToString(); }
		}

		public bool GenerateDebugInformation
		{
			get { return Convert.ToBoolean(generateDebugInformation.InnerText); }
			set { generateDebugInformation.InnerText = value.ToString(); }
		}

		public Optimizations Optimization
		{
			get { return (Optimizations)Enum.Parse(typeof(Optimizations), optimization.InnerText); }
			set { optimization.InnerText = value.ToString(); }
		}

		public bool MinimalRebuild
		{
			get { return Convert.ToBoolean(minimalRebuild.InnerText); }
			set { minimalRebuild.InnerText = value.ToString(); }
		}

		public VCProjectFileGenerator()
		{
			XmlElement itemGroup = CreateElement("ItemGroup");
			projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
			configuration = CreateElement("Configuration", projectConfiguration);
			platform = CreateElement("Platform", projectConfiguration);

			XmlElement import = CreateElement("Import");
			import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.default.props");

			XmlElement popertyGroup = CreateElement("PropertyGroup");
			configurationType = CreateElement("ConfigurationType", popertyGroup);
			platformToolset = CreateElement("PlatformToolset", popertyGroup);

			popertyGroup = CreateElement("PropertyGroup");
			outDir = CreateElement("OutDir", popertyGroup);
			targetName = CreateElement("TargetName", popertyGroup);

			XmlElement clCompile = CreateElement("ClCompile", CreateElement("ItemDefinitionGroup"));
			runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);

			import = CreateElement("Import");
			import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

			XmlElement itemDefinitionGroup = CreateElement("ItemDefinitionGroup");
			clCompile = CreateElement("ClCompile", itemDefinitionGroup);
			additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
			preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
			optimization = CreateElement("Optimization", clCompile);
			minimalRebuild = CreateElement("MinimalRebuild", clCompile);
			link = CreateElement("Link", itemDefinitionGroup);
			generateDebugInformation = CreateElement("GenerateDebugInformation", link);
			additionalLibraryDirectories = CreateElement("AdditionalLibraryDirectories", link);
			additionalLibraries = CreateElement("AdditionalDependencies", link);

			includeFiles = CreateElement("ItemGroup");
			compileFiles = CreateElement("ItemGroup");

			import = CreateElement("Import");
			import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.Targets");

			ToolsVersion = ToolsVersions.v14_0;
		}

		public void AddIncludeFile(string FilePath)
		{
			XmlElement include = CreateElement("ClInclude", includeFiles);
			include.SetAttribute("Include", FilePath);
		}

		public override void AddCompileFile(string FilePath)
		{
			XmlElement include = CreateElement("ClCompile", compileFiles);
			include.SetAttribute("Include", FilePath);
		}

		public override void Generate(string Path)
		{
			if (!PreprocessorDefinitions.Contains("%(PreprocessorDefinitions)"))
				PreprocessorDefinitions += "%(PreprocessorDefinitions)";

			if (!IncludeDirectories.Contains("%(AdditionalIncludeDirectories)"))
				IncludeDirectories += "%(AdditionalIncludeDirectories)";

			if (!IncludeLibraryDirectories.Contains("%(AdditionalLibraryDirectories)"))
				IncludeLibraryDirectories += "%(AdditionalLibraryDirectories)";

			if (!IncludeLibraries.Contains("%(AdditionalDependencies)"))
				IncludeLibraries += "%(AdditionalDependencies)";

			base.Generate(Path);
		}
	}
}
