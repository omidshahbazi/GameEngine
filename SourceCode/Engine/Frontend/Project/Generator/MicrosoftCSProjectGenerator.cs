// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend.Project.Generator
{
	//https://msdn.microsoft.com/en-us/library/dd576348.aspx

	class MicrosoftCSProjectGenerator : MSBuildProjectGenerator
	{
		public override string Generate(ProjectBase Project, bool WithBeutyConfigurationName)
		{
			CSProject project = (CSProject)Project;
			CSProject.Profile profile = (CSProject.Profile)project.Profiles[0];

			XmlElement projectElement = CreateProjectElement();
			{
				XmlElement propertyGroup = CreateElement("PropertyGroup", projectElement);
				{
					SetElementValue("AssemblyName", profile.AssemblyName, propertyGroup);
					SetElementValue("OutputPath", profile.OutputPath, propertyGroup);
					SetElementValue("OutputType", GetOutputType(profile), propertyGroup);
					SetElementValue("DefineConstants", GetFlattenStringList(profile.PreprocessorDefinitions), propertyGroup);
					SetElementValue("TargetFrameworkVersion", profile.FrameworkVersion.ToString().Replace('_', '.'), propertyGroup);
				}

				XmlElement target = CreateElement("Target", projectElement);
				{
					target.SetAttribute("Name", "Build");

					XmlElement makeDir = CreateElement("MakeDir", target);
					{
						makeDir.SetAttribute("Directories", "$(OutputPath)");
						makeDir.SetAttribute("Condition", "!Exists('$(OutputPath)')");

						makeDir = CreateElement("Csc", target);
						makeDir.SetAttribute("Sources", "@(Compile)");
						makeDir.SetAttribute("OutputAssembly", "$(OutputPath)$(AssemblyName)");

						target = CreateElement("Target", projectElement);
						{
							target.SetAttribute("Name", "Clean");

							makeDir = CreateElement("Delete", target);
							makeDir.SetAttribute("Files", "$(OutputPath)$(AssemblyName)");

							target = CreateElement("Target", projectElement);
							{
								target.SetAttribute("Name", "Rebuild");
								target.SetAttribute("DependsOnTargets", "Clean;Build");
							}
						}
					}

					XmlElement import = CreateElement("Import", projectElement);
					import.SetAttribute("Project", "$(MSBuildToolsPath)/Microsoft.CSharp.targets");

					XmlElement referencesItemGroup = CreateElement("ItemGroup", projectElement);
					AddStringListToEllementAsAttribute(referencesItemGroup, "Reference", "Include", project.ReferenceBinaryFiles);

					XmlElement compilesItemGroup = CreateElement("ItemGroup", projectElement);
					AddStringListToEllementAsAttribute(compilesItemGroup, "Compile", "Include", project.CompileFiles);
				}
			}

			return projectElement.OwnerDocument.OuterXml;
		}

		private string GetOutputType(ProjectBase.ProfileBase Profile)
		{
			string type = "";

			switch (Profile.OutputType)
			{
				case ProjectBase.ProfileBase.OutputTypes.Application:
					type = "Exe";
					break;
				case ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary:
					type = "Library";
					break;
				default:
					throw new Exception("[" + Profile.OutputType + "] doesn't supported by [" + GetType().Name + "]");
			}

			return type;
		}
	}
}