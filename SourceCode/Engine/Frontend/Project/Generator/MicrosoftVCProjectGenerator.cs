// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.System;
using Engine.Frontend.Utilities;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace Engine.Frontend.Project.Generator
{
    //https://msdn.microsoft.com/en-us/library/dd293607.aspx

    class MicrosoftVCProjectGenerator : MSBuildProjectGenerator
    {
        public enum ToolsVersions
        {
            v14_0,
            v14_1
        }

        public ToolsVersions ToolsVersion
        {
            get;
            set;
        }

        private static string GetInstalledWindowsKitVersion
        {
            get
            {
                const string PATH = @"SOFTWARE\Microsoft\Windows Kits\Installed Roots\";

                RegistryKey registry = Registry.LocalMachine.OpenSubKey(PATH);

                string[] versions = registry.GetSubKeyNames();

                return versions[0];
            }
        }

        public override string Generate(ProjectBase Project)
        {
            CPPProject project = (CPPProject)Project;

            XmlElement projectElement = CreateProjectElement();
            {
                XmlElement winSDKVersion = CreateElement("WindowsTargetPlatformVersion", CreateElement("PropertyGroup", projectElement));
                winSDKVersion.InnerText = GetInstalledWindowsKitVersion;

                projectElement.SetAttribute("ToolsVersion", ToolsVersion.ToString().Substring(1).Replace('_', '.'));

                XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
                {
                    for (int i = 0; i < project.Profiles.Length; ++i)
                    {
                        XmlElement projectConfiguration = CreateElement("ProjectConfiguration", itemGroup);
                        {
                            CPPProject.Profile profile = (CPPProject.Profile)project.Profiles[i];

                            projectConfiguration.SetAttribute("Include", GetConfiguration(profile));

                            XmlElement configuration = CreateElement("Configuration", projectConfiguration);
                            configuration.InnerText = profile.BuildConfiguration.ToString() + (string.IsNullOrEmpty(profile.Name) ? "" : " " + profile.Name);

                            XmlElement platform = CreateElement("Platform", projectConfiguration);
                            platform.InnerText = GetPlatformType(profile);
                        }
                    }
                }

                XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttribute(includeFiles, "ClInclude", "Include", project.IncludeFiles);

                XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttribute(includeFiles, "ClCompile", "Include", project.CompileFiles);

                XmlElement noneFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttribute(noneFiles, "None", "Include", project.ExtraFiles);

                XmlElement import = CreateElement("Import", projectElement);
                import.SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");

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
                    }

                    popertyGroup = CreateElement("PropertyGroup", projectElement);
                    {
                        popertyGroup.SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='" + GetConfiguration(profile) + "'");

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
                        }
                        else
                        {
                            XmlElement outDir = CreateElement("OutDir", popertyGroup);
                            outDir.InnerText = profile.OutputPath;

                            XmlElement targetName = CreateElement("TargetName", popertyGroup);
                            targetName.InnerText = profile.AssemblyName;
                        }

                        XmlElement intDir = CreateElement("IntDir", popertyGroup);
                        intDir.InnerText = profile.IntermediatePath;
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
                        itemDefinitionGroup.SetAttribute("Condition", "'$(Configuration)|$(Platform)'=='" + GetConfiguration(profile) + "'");

                        XmlElement clCompile = CreateElement("ClCompile", itemDefinitionGroup);
                        {
                            XmlElement runtimeLibrary = CreateElement("RuntimeLibrary", clCompile);
                            runtimeLibrary.InnerText = profile.RuntimeLibrary.ToString();

                            XmlElement additionalIncludeDirectories = CreateElement("AdditionalIncludeDirectories", clCompile);
                            if (Array.IndexOf(profile.IncludeDirectories, "%(AdditionalIncludeDirectories)") == -1)
                                profile.AddIncludeDirectories("%(AdditionalIncludeDirectories)");
                            additionalIncludeDirectories.InnerText = GetFlattenStringList(profile.IncludeDirectories);
                            additionalIncludeDirectories.InnerText += GetFlattenStringList(profile.IncludeLibraryDirectories);

                            XmlElement preprocessorDefinitions = CreateElement("PreprocessorDefinitions", clCompile);
                            if (Array.IndexOf(profile.PreprocessorDefinitions, "%(PreprocessorDefinitions)") == -1)
                                profile.AddPreprocessorDefinition("%(PreprocessorDefinitions)");
                            preprocessorDefinitions.InnerText = GetFlattenStringList(profile.PreprocessorDefinitions);

                            XmlElement optimization = CreateElement("Optimization", clCompile);
                            optimization.InnerText = profile.Optimization.ToString();

                            XmlElement minimalRebuild = CreateElement("MinimalRebuild", clCompile);
                            minimalRebuild.InnerText = profile.MinimalRebuild.ToString();
                        }

                        XmlElement importTargets = CreateElement("Import", projectElement);
                        importTargets.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.props");

                        XmlElement link = CreateElement("Link", itemDefinitionGroup);
                        {
                            XmlElement generateDebugInformation = CreateElement("GenerateDebugInformation", link);
                            generateDebugInformation.InnerText = profile.GenerateDebugInformation.ToString();

                            XmlElement additionalLibraryDirectories = CreateElement("AdditionalLibraryDirectories", link);
                            if (Array.IndexOf(profile.IncludeLibraryDirectories, "%(AdditionalLibraryDirectories)") == -1)
                                profile.AddIncludeLibraryDirectories("%(AdditionalLibraryDirectories)");
                            additionalLibraryDirectories.InnerText = GetFlattenStringList(profile.AdditionalLibraryDirectories);

                            XmlElement additionalLibraries = CreateElement("AdditionalDependencies", link);
                            if (Array.IndexOf(profile.IncludeLibraries, "%(AdditionalDependencies)") == -1)
                                profile.AddIncludeLibraries("%(AdditionalDependencies)");
                            additionalLibraries.InnerText = GetFlattenStringList(profile.IncludeLibraries);
                        }
                    }
                }

                import = CreateElement("Import", projectElement);
                import.SetAttribute("Project", "$(VCTargetsPath)/Microsoft.Cpp.targets");
            }

            return projectElement.OwnerDocument.OuterXml;
        }

        public virtual string GenerateFilter(ProjectBase Project, string RootPath)
        {
            CPPProject project = (CPPProject)Project;

            XmlDocument document = new XmlDocument();

            XmlElement projectElement = document.CreateElement("Project");
            {
                document.AppendChild(projectElement);

                projectElement.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
                projectElement.SetAttribute("ToolsVersion", "4.0");

                XmlElement itemGroup = CreateElement("ItemGroup", projectElement);
                {
                    List<string> files = new List<string>();
                    files.AddRange(project.IncludeFiles);
                    files.AddRange(project.CompileFiles);
                    files.AddRange(project.ExtraFiles);

                    List<string> filtersName = new List<string>();

                    foreach (string file in files)
                    {
                        string filterName = GetFilterName(file, RootPath);

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
                            identifier.InnerText = "{" + Guid.NewGuid() + "}";
                        }
                    }
                }

                XmlElement includeFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttributeAndFilter(includeFiles, "ClInclude", "Include", project.IncludeFiles, RootPath);

                XmlElement compileFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttributeAndFilter(includeFiles, "ClCompile", "Include", project.CompileFiles, RootPath);

                XmlElement noneFiles = CreateElement("ItemGroup", projectElement);
                AddStringListToEllementAsAttributeAndFilter(noneFiles, "None", "Include", project.ExtraFiles, RootPath);
            }

            return projectElement.OwnerDocument.OuterXml;
        }

        private static string GetConfiguration(CPPProject.Profile Profile)
        {
            //return Profile.BuildConfiguration.ToString() + "|" + GetPlatformType(Profile);
            return Profile.BuildConfiguration.ToString() + (string.IsNullOrEmpty(Profile.Name) ? "" : " " + Profile.Name) + "|" + GetPlatformType(Profile);
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
            return Path.GetDirectoryName(FullPath).Replace(RootPath, string.Empty);
        }
    }
}