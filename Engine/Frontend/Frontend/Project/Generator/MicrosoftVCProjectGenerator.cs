// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace Engine.Frontend.Project.Generator
{
	//https://msdn.microsoft.com/en-us/library/dd293607.aspx

	class MicrosoftVCProjectGenerator : MSBuildProjectGenerator
	{
		public enum ToolsVersions
		{
			v14_0,
			v14_1,
			v14_2,
			v14_3
		}

		public ToolsVersions ToolsVersion
		{
			get;
			set;
		}

		private static string InstalledWindowsKitVersion
		{
			get
			{
				const string PATH = @"SOFTWARE\Microsoft\Windows Kits\Installed Roots\";

				RegistryKey registry = Registry.LocalMachine.OpenSubKey(PATH);

				string[] versions = registry.GetSubKeyNames();

				return (versions.Length == 0 ? "" : versions[versions.Length - 1]);
			}
		}

		public override string Generate(ProjectBase Project, bool WithBeutyConfigurationName)
		{
			CPPProject project = (CPPProject)Project;

			XmlElement projectElement = CreateProjectElement();
			{
				if (ToolsVersion >= ToolsVersions.v14_1)
				{
					XmlElement winSDKVersion = CreateElement("WindowsTargetPlatformVersion", CreateElement("PropertyGroup", projectElement));
					winSDKVersion.InnerText = InstalledWindowsKitVersion;
				}

				projectElement.SetAttribute("ToolsVersion", ToolsVersion.ToString().Substring(1).Replace('_', '.'));

				XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
				{
					for (int i = 0; i < project.Profiles.Length; ++i)
					{
						XmlElement projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
						{
							CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

							projectConfiguration.SetAttribute("Include", GetConfigurationAndPlatform(profile, WithBeutyConfigurationName));

							XmlElement configuration = CreateElement("Configuration", projectConfiguration);
							configuration.InnerText = GetConfiguration(profile, WithBeutyConfigurationName);

							XmlElement platform = CreateElement("Platform", projectConfiguration);
							platform.InnerText = GetPlatformType(profile.PlatformArchitecture);
						}
					}
				}

				XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClInclude", "Include", project.IncludeFiles);

				XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(includeFiles, "ClCompile", "Include", project.CompileFiles);

				XmlElement resourceFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(resourceFiles, "ResourceCompile", "Include", project.Resources);

				XmlElement noneFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttribute(noneFiles, "None", "Include", project.ExtraFiles);

				XmlElement import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");

				for (int i = 0; i < project.Profiles.Length; ++i)
				{
					CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

					XmlElement popertyGroup = CreateElement("PropertyGroup", projectElement);
					{
						popertyGroup.SetAttribute("Condition", $"'$(Configuration)|$(Platform)'=='{GetConfigurationAndPlatform(profile, WithBeutyConfigurationName)}'");

						XmlElement configurationType = CreateElement("ConfigurationType", popertyGroup);
						configurationType.InnerText = GetOutputType(profile);

						XmlElement platformToolset = CreateElement("PlatformToolset", popertyGroup);
						platformToolset.InnerText = ToolsVersion.ToString().Replace("_", "");
					}

					popertyGroup = CreateElement("PropertyGroup", projectElement);
					{
						popertyGroup.SetAttribute("Condition", $"'$(Configuration)|$(Platform)'=='{GetConfigurationAndPlatform(profile, WithBeutyConfigurationName)}'");

						if (profile.OutputType == ProjectBase.ProfileBase.OutputTypes.Makefile)
						{
							XmlElement nmakeBuildCommandLine = CreateElement("NMakeBuildCommandLine", popertyGroup);
							nmakeBuildCommandLine.InnerText = profile.NMakeBuildCommandLine;

							XmlElement nmakeRebuildCommandLine = CreateElement("NMakeReBuildCommandLine", popertyGroup);
							nmakeRebuildCommandLine.InnerText = profile.NMakeReBuildCommandLine;

							XmlElement nmakeCleanCommandLine = CreateElement("NMakeCleanCommandLine", popertyGroup);
							nmakeCleanCommandLine.InnerText = profile.NMakeCleanCommandLine;

							XmlElement nmakeIncludeSearchPath = CreateElement("NMakeIncludeSearchPath", popertyGroup);
							nmakeIncludeSearchPath.InnerText = GetFlattenStringList(profile.IncludeDirectories);

							XmlElement nmakeOutput = CreateElement("NMakeOutput", popertyGroup);
							nmakeOutput.InnerText = profile.OutputPath;

							XmlElement nmakePreprocessorDefinitions = CreateElement("NMakePreprocessorDefinitions", popertyGroup);
							nmakePreprocessorDefinitions.InnerText = GetFlattenStringList(profile.PreprocessorDefinitions);

							XmlElement additionalOptions = CreateElement("AdditionalOptions", popertyGroup);
							additionalOptions.InnerText = $"/std:{profile.LanguageStandard.ToString().ToLower().Replace("pp", "++")}";
						}
						else
						{
							XmlElement outDir = CreateElement("OutDir", popertyGroup);
							outDir.InnerText = profile.OutputPath;

							XmlElement targetName = CreateElement("TargetName", popertyGroup);
							targetName.InnerText = profile.AssemblyName;
						}

						XmlElement intDir = CreateElement("IntDir", popertyGroup);
						intDir.InnerText = profile.IntermediateDirectory;
					}
				}

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");

				for (int i = 0; i < project.Profiles.Length; ++i)
				{
					CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

					if (profile.OutputType == ProjectBase.ProfileBase.OutputTypes.Makefile)
						continue;

					XmlElement itemDefinitionGroup = CreateElement("ItemDefinitionGroup", projectElement);
					{
						itemDefinitionGroup.SetAttribute("Condition", $"'$(Configuration)|$(Platform)'=='{GetConfigurationAndPlatform(profile, WithBeutyConfigurationName)}'");

						XmlElement clCompile = CreateElement("ClCompile", itemDefinitionGroup);
						{
							XmlElement runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);
							runtimeLibrary.InnerText = profile.RuntimeLibrary.ToString();

							XmlElement additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
							if (Array.IndexOf(profile.IncludeDirectories, "%(AdditionalIncludeDirectories)") == -1)
								profile.AddIncludeDirectory("%(AdditionalIncludeDirectories)");
							additionalIncludeDirectories.InnerText = GetFlattenStringList(profile.IncludeDirectories);

							XmlElement preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
							if (Array.IndexOf(profile.PreprocessorDefinitions, "%(PreprocessorDefinitions)") == -1)
								profile.AddPreprocessorDefinition("%(PreprocessorDefinitions)");
							preprocessorDefinitions.InnerText = GetFlattenStringList(profile.PreprocessorDefinitions);

							XmlElement optimization = CreateElement("Optimization", clCompile);
							optimization.InnerText = profile.Optimization.ToString();

							XmlElement minimalRebuild = CreateElement("MinimalRebuild", clCompile);
							minimalRebuild.InnerText = profile.MinimalRebuild.ToString();

							XmlElement languageStandard = CreateElement("LanguageStandard", clCompile);
							languageStandard.InnerText = $"std{profile.LanguageStandard.ToString().ToLower()}";
						}

						XmlElement importTargets = CreateElement("Import", projectElement);
						importTargets.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

						XmlElement link = CreateElement("Link", itemDefinitionGroup);
						{
							XmlElement generateDebugInfo = CreateElement("GenerateDebugInformation", link);
							generateDebugInfo.InnerText = profile.GenerateDebugInfo.ToString();

							XmlElement additionalLibraries = CreateElement("AdditionalDependencies", link);
							if (Array.IndexOf(profile.IncludeLibraries, "%(AdditionalDependencies)") == -1)
								profile.AddIncludeLibrary("%(AdditionalDependencies)");
							additionalLibraries.InnerText = GetFlattenStringList(profile.IncludeLibraries);
						}
					}
				}

				import = CreateElement("Import", projectElement);
				import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.targets");
			}

			return projectElement.OwnerDocument.OuterXml;
		}

		public virtual string GenerateFilter(CPPProject Project, string RootPath)
		{
			XmlDocument document = new XmlDocument();

			XmlElement projectElement = document.CreateElement("Project");
			{
				document.AppendChild(projectElement);

				projectElement.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
				projectElement.SetAttribute("ToolsVersion", "4.0");

				XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
				{
					List<string> files = new List<string>();
					files.AddRange(Project.IncludeFiles);
					files.AddRange(Project.CompileFiles);
					files.AddRange(Project.ExtraFiles);

					List<string> filtersName = new List<string>();

					foreach (string file in files)
					{
						string filterName = GetFilterName(file, RootPath);
						if (string.IsNullOrEmpty(filterName))
							continue;

						string[] parts = filterName.Split(EnvironmentHelper.PathSeparator);

						string filter = string.Empty;
						for (int i = 0; i < parts.Length; ++i)
						{
							if (i != 0)
								filter += EnvironmentHelper.PathSeparator;

							filter += parts[i];

							if (filtersName.Contains(filter))
								continue;

							filtersName.Add(filter);
						}
					}

					foreach (string filterName in filtersName)
					{
						XmlElement filter = CreateElement("Filter", itemGroup);
						{
							filter.SetAttribute("Include", filterName);

							XmlElement identifier = CreateElement("UniqueIdentifier", filter);
							identifier.InnerText = $"{{{Guid.NewGuid()}}}";
						}
					}
				}

				XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttributeAndFilter(includeFiles, "ClInclude", "Include", Project.IncludeFiles, RootPath);

				XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttributeAndFilter(includeFiles, "ClCompile", "Include", Project.CompileFiles, RootPath);

				XmlElement noneFiles = CreateElement("ItemGroup", projectElement);
				AddStringListToEllementAsAttributeAndFilter(noneFiles, "None", "Include", Project.ExtraFiles, RootPath);
			}

			return projectElement.OwnerDocument.OuterXml;
		}

		public virtual string GenerateResourceDefinition(CPPProject Project)
		{
			ResourceDefinition resourceDefinition = Project.ResourceDefinition;

			if (resourceDefinition == null)
				return null;

			string commaSeparatedVersion = resourceDefinition.ProductVersion.ToString().Replace('.', ',');

			StringBuilder builder = new StringBuilder();

			builder.AppendLine("#include \"winres.h\"");
			builder.AppendLine("VS_VERSION_INFO VERSIONINFO");
			{
				builder.AppendFormat(" FILEVERSION {0}\n", commaSeparatedVersion);
				builder.AppendFormat(" PRODUCTVERSION {0}\n", commaSeparatedVersion);
				builder.AppendLine(" FILEFLAGSMASK 0x3fL");

				builder.AppendLine("#ifdef _DEBUG");
				builder.AppendLine(" FILEFLAGS 0x1L");
				builder.AppendLine("#else");
				builder.AppendLine(" FILEFLAGS 0x0L");
				builder.AppendLine("#endif");

				builder.AppendLine(" FILEOS 0x40004L");
				builder.AppendLine(" FILETYPE 0x1L");
				builder.AppendLine(" FILESUBTYPE 0x0L");

				builder.AppendLine("BEGIN");
				{
					builder.AppendLine("	BLOCK \"StringFileInfo\"");
					builder.AppendLine("	BEGIN");
					{
						builder.AppendLine("		BLOCK \"040904b0\"");
						builder.AppendLine("		BEGIN");
						{
							builder.AppendFormat("			VALUE \"CompanyName\", \"{0}\"\n", resourceDefinition.CompanyName);
							builder.AppendFormat("			VALUE \"ProductName\", \"{0}\"\n", resourceDefinition.ProductName);
							builder.AppendFormat("			VALUE \"ProductVersion\", \"{0}\"\n", resourceDefinition.ProductVersion);
							builder.AppendFormat("			VALUE \"FileDescription\", \"{0}\"\n", resourceDefinition.ProductDescription);
							builder.AppendFormat("			VALUE \"LegalCopyright\", \"{0}\"\n", resourceDefinition.Copyright);
						}
						builder.AppendLine("		END");

					}
					builder.AppendLine("	END");

					builder.AppendLine("	BLOCK \"VarFileInfo\"");
					builder.AppendLine("	BEGIN");
					{
						builder.AppendLine("		VALUE \"Translation\", 0x409, 1200");
					}
					builder.AppendLine("	END");
				}
				builder.AppendLine("END");
			}

			if (!string.IsNullOrEmpty(resourceDefinition.IconPath))
				builder.AppendFormat("IDI_ICON1               ICON                    \"{0}\"\n", resourceDefinition.IconPath);

			return builder.ToString();
		}

		public static string GetPlatformType(ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			switch (Architecture)
			{
				case CPPProject.Profile.PlatformArchitectures.x86:
					return "Win32";

				case CPPProject.Profile.PlatformArchitectures.x64:
					return "x64";

				default:
					throw new NotImplementedException($"Handler for {Architecture} has not implemented");
			}
		}

		private static string GetConfiguration(CPPProject.Profile Profile, bool WithBeutyConfigurationName)
		{
			if (WithBeutyConfigurationName)
				return Profile.BuildConfiguration + (string.IsNullOrEmpty(Profile.Name) ? "" : $" {Profile.Name}");

			return Profile.BuildConfiguration.ToString();
		}

		private static string GetConfigurationAndPlatform(CPPProject.Profile Profile, bool WithBeutyConfigurationName)
		{
			return $"{GetConfiguration(Profile, WithBeutyConfigurationName)}|{GetPlatformType(Profile.PlatformArchitecture)}";
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
				case ProjectBase.ProfileBase.OutputTypes.Makefile:
					type = "Makefile";
					break;
			}

			return type;
		}

		private void AddStringListToEllementAsAttributeAndFilter(XmlElement Parent, string ElementName, string AttributeName, IEnumerable<string> List, string RootPath)
		{
			IEnumerator<string> it = List.GetEnumerator();

			while (it.MoveNext())
			{
				XmlElement elem = CreateElement(ElementName, Parent);
				{
					elem.SetAttribute(AttributeName, it.Current);

					XmlElement filter = CreateElement("Filter", elem);
					filter.InnerText = GetFilterName(it.Current, RootPath);
				}
			}
		}

		private static string GetFilterName(string FullPath, string RootPath)
		{
			return Path.GetDirectoryName(FullPath.Replace(RootPath, string.Empty));
		}
	}
}