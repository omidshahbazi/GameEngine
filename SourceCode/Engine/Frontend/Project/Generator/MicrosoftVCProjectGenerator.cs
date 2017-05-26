// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend.Project.Generator
{
	//https://msdn.microsoft.com/en-us/library/dd293607.aspx

	class MicrosoftVCProjectGenerator : MSBuildProjectGenerator
	{
		public enum ToolsVersions
		{
			v9_0 = 0,
			v10_0,
			v11_0,
			v12_0,
			v14_0
		}

		public ToolsVersions ToolsVersion
		{
			get;
			set;
		}

		public override string Generate(ProjectBase Project)
		{
			CPPProject project = (CPPProject)Project;

			XmlElement projectElement = CreateProjectElement();
			{
				projectElement.SetAttribute("ToolsVersion", ToolsVersion.ToString().Substring(1).Replace('_', '.'));

				XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
				{
					XmlElement projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
					{
						for (int i = 0; i < project.Profiles.Length; ++i)
						{
							CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

							projectConfiguration.SetAttribute("Include", GetConfiguration(profile));

							XmlElement configuration = CreateElement("Configuration", projectConfiguration);
							configuration.InnerText = profile.BuildConfiguration.ToString();

							XmlElement platform = CreateElement("Platform", projectConfiguration);
							platform.InnerText = GetPlatformType(profile);
						}
					}
				}

				XmlElement import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.default.props");

				for (int i = 0; i < project.Profiles.Length; ++i)
				{
					CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

					XmlElement popertyGroup = CreateElement("PropertyGroup", projectElement);
					{
						popertyGroup.SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='" + GetConfiguration(profile) + "'");

						XmlElement configurationType = CreateElement("ConfigurationType", popertyGroup);
						configurationType.InnerText = GetOutputType(profile);

						XmlElement platformToolset = CreateElement("PlatformToolset", popertyGroup);
						platformToolset.InnerText = ToolsVersion.ToString().Replace("_", "");

						popertyGroup = CreateElement("PropertyGroup", projectElement);

						XmlElement outDir = CreateElement("OutDir", popertyGroup);
						outDir.InnerText = profile.OutputPath;

						XmlElement targetName = CreateElement("TargetName", popertyGroup);
						targetName.InnerText = profile.AssemblyName;
					}
				}

				for (int i = 0; i < project.Profiles.Length; ++i)
				{
					CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

					XmlElement itemDefinitionGroup = CreateElement("ItemDefinitionGroup", projectElement);
					{
						itemDefinitionGroup.SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='" + GetConfiguration(profile) + "'");

						XmlElement clCompile = CreateElement("ClCompile", itemDefinitionGroup);
						{
							XmlElement runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);
							runtimeLibrary.InnerText = profile.RuntimeLibrary.ToString();

							import = CreateElement("Import", projectElement);
							import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

							//clCompile = CreateElement("ClCompile", itemDefinitionGroup);

							XmlElement additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
							if (Array.IndexOf(profile.IncludeDirectories, "%(AdditionalIncludeDirectories)") == -1)
								profile.AddIncludeDirectories("%(AdditionalIncludeDirectories)");
							additionalIncludeDirectories.InnerText = GetFlattenStringList(profile.IncludeDirectories);

							XmlElement preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
							if (Array.IndexOf(profile.PreprocessorDefinitions, "%(PreprocessorDefinitions)") == -1)
								profile.AddPreprocessorDefinition("%(PreprocessorDefinitions)");
							preprocessorDefinitions.InnerText = GetFlattenStringList(profile.PreprocessorDefinitions);

							XmlElement optimization = CreateElement("Optimization", clCompile);
							optimization.InnerText = profile.Optimization.ToString();

							XmlElement minimalRebuild = CreateElement("MinimalRebuild", clCompile);
							minimalRebuild.InnerText = profile.MinimalRebuild.ToString();
						}

						XmlElement link = CreateElement("Link", itemDefinitionGroup);
						{
							XmlElement generateDebugInformation = CreateElement("GenerateDebugInformation", link);
							generateDebugInformation.InnerText = profile.GenerateDebugInformation.ToString();

							XmlElement additionalLibraryDirectories = CreateElement("AdditionalLibraryDirectories", link);
							if (Array.IndexOf(profile.IncludeLibraryDirectories, "%(AdditionalLibraryDirectories)") == -1)
								profile.AddIncludeLibraryDirectories("%(AdditionalLibraryDirectories)");
							additionalLibraryDirectories.InnerText = GetFlattenStringList(profile.AdditionalLibraryDirectories);
							additionalLibraryDirectories.InnerText += GetFlattenStringList(profile.IncludeLibraryDirectories);

							XmlElement additionalLibraries = CreateElement("AdditionalDependencies", link);
							if (Array.IndexOf(profile.IncludeLibraries, "%(AdditionalDependencies)") == -1)
								profile.AddIncludeLibraries("%(AdditionalDependencies)");
							additionalLibraries.InnerText = GetFlattenStringList(profile.IncludeLibraries);
						}
					}
				}

				XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClInclude", "Include", project.IncludeFiles);

				XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClCompile", "Include", project.CompileFiles);

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.Targets");
			}

			return projectElement.OwnerDocument.OuterXml;
		}

		private static string GetConfiguration(CPPProject.Profile Profile)
		{
			return Profile.BuildConfiguration.ToString() + "|" + GetPlatformType(Profile);
		}

		private static string GetPlatformType(CPPProject.Profile Profile)
		{
			string type = "";

			switch (Profile.PlatformType)
			{
				case CPPProject.Profile.PlatformTypes.x86:
					type = "Win32";
					break;
				case CPPProject.Profile.PlatformTypes.x64:
					type = "x64";
					break;
			}

			return type;
		}

		private string GetOutputType(ProjectBase.ProfileBase Profile)
		{
			string type = "";

			switch (Profile.OutputType)
			{
				case ProjectBase.ProfileBase.OutputTypes.Application:
					type = "Application";
					break;
				case ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary:
					type = "DynamicLibrary";
					break;
				case ProjectBase.ProfileBase.OutputTypes.StaticLinkLibrary:
					type = "StaticLibrary";
					break;
			}

			return type;
		}
	}
}