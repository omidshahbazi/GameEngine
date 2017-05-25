// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend.Project.Generator
{
	//https://msdn.microsoft.com/en-us/library/dd293607.aspx

	class MicrosoftVCProjectGenerator : MSBuildProjectGenerator
	{
		public override string Generate(ProjectBase Project)
		{
			CPPProject project = (CPPProject)Project;

			XmlElement projectElement = CreateProjectElement();
			{
				projectElement.SetAttribute("ToolsVersion", project.ToolsVersion.ToString().Substring(1).Replace('_', '.'));

				XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
				{
					XmlElement projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
					{
						projectConfiguration.SetAttribute("Include", project.BuildConfiguration.ToString() + "|" + project.PlatformType.ToString());

						XmlElement configuration = CreateElement("Configuration", projectConfiguration);
						configuration.InnerText = project.BuildConfiguration.ToString();

						XmlElement platform = CreateElement("Platform", projectConfiguration);
						platform.InnerText = project.PlatformType.ToString();
					}
				}

				XmlElement import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.default.props");

				XmlElement popertyGroup = CreateElement("PropertyGroup", projectElement);
				{
					XmlElement configurationType = CreateElement("ConfigurationType", popertyGroup);
					configurationType.InnerText = GetOutputType(project);

					XmlElement platformToolset = CreateElement("PlatformToolset", popertyGroup);
					platformToolset.InnerText = project.ToolsVersion.ToString().Replace("_", "");

					popertyGroup = CreateElement("PropertyGroup", projectElement);

					XmlElement outDir = CreateElement("OutDir", popertyGroup);
					outDir.InnerText = project.OutputPath;

					XmlElement targetName = CreateElement("TargetName", popertyGroup);
					targetName.InnerText = project.AssemblyName;
				}

				XmlElement clCompile = CreateElement("ClCompile", CreateElement("ItemDefinitionGroup", projectElement));
				{
					XmlElement runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);
					runtimeLibrary.InnerText = project.RuntimeLibrary.ToString();

					import = CreateElement("Import", projectElement);
					import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

					XmlElement itemDefinitionGroup = CreateElement("ItemDefinitionGroup", projectElement);
					{
						clCompile = CreateElement("ClCompile", itemDefinitionGroup);
						{
							XmlElement additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
							additionalIncludeDirectories.InnerText = GetFlattenStringList(project.IncludeDirectories);

							XmlElement preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
							preprocessorDefinitions.InnerText = GetFlattenStringList(project.PreprocessorDefinitions);

							XmlElement optimization = CreateElement("Optimization", clCompile);
							optimization.InnerText = project.Optimization.ToString();

							XmlElement minimalRebuild = CreateElement("MinimalRebuild", clCompile);
							minimalRebuild.InnerText = project.MinimalRebuild.ToString();
						}

						XmlElement link = CreateElement("Link", itemDefinitionGroup);
						{
							XmlElement generateDebugInformation = CreateElement("GenerateDebugInformation", link);
							generateDebugInformation.InnerText = project.GenerateDebugInformation.ToString();

							XmlElement additionalLibraryDirectories = CreateElement("AdditionalLibraryDirectories", link);
							additionalLibraryDirectories.InnerText = GetFlattenStringList(project.AdditionalLibraryDirectories);
							additionalLibraryDirectories.InnerText = GetFlattenStringList(project.IncludeLibraryDirectories);

							XmlElement additionalLibraries = CreateElement("AdditionalDependencies", link);
							additionalLibraries.InnerText = GetFlattenStringList(project.IncludeLibraries);
						}
					}
				}

				XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClInclude", "Include", project.IncludeFiles);

				XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClCompile", "Include", project.CompileFiles);

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.Targets");

				if (Array.IndexOf(project.PreprocessorDefinitions, "%(PreprocessorDefinitions)") == -1)
					project.AddPreprocessorDefinition("%(PreprocessorDefinitions)");

				if (Array.IndexOf(project.IncludeDirectories, "%(AdditionalIncludeDirectories)") == -1)
					project.AddIncludeDirectories("%(AdditionalIncludeDirectories)");

				if (Array.IndexOf(project.IncludeLibraryDirectories, "%(AdditionalLibraryDirectories)") == -1)
					project.AddIncludeLibraryDirectories("%(AdditionalLibraryDirectories)");

				if (Array.IndexOf(project.IncludeLibraries, "%(AdditionalDependencies)") == -1)
					project.AddIncludeLibraries("%(AdditionalDependencies)");
			}

			return projectElement.OwnerDocument.OuterXml;
		}

		private string GetOutputType(ProjectBase Project)
		{
			string type = "";

			switch (Project.OutputType)
			{
				case ProjectBase.OutputTypes.Application:
					type = "Application";
					break;
				case ProjectBase.OutputTypes.DynamicLinkLibrary:
					type = "DynamicLibrary";
					break;
				case ProjectBase.OutputTypes.StaticLinkLibrary:
					type = "StaticLibrary";
					break;
			}

			return type;
		}
	}
}