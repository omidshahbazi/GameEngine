// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend.ProjectFile
{
	//https://msdn.microsoft.com/en-us/library/dd576348.aspx

	class CSProjectFile : MSBuildProjectFile
	{
		public enum FrameworkVersions
		{
			v2_0 = 0,
			v3_0,
			v3_5,
			v4_0,
			v4_5,
			v4_5_1
		}

		private StringList referenceBinaryFiles = new StringList();

		public FrameworkVersions FrameworkVersion
		{
			get;
			set;
		}

		public override string Content
		{
			get
			{
				XmlElement projectElement = CreateProjectElement();
				{
					XmlElement propertyGroup = CreateElement("PropertyGroup", projectElement);
					{
						SetElementValue("AssemblyName", AssemblyName, propertyGroup);
						SetElementValue("OutputPath", OutputPath, propertyGroup);
						SetElementValue("OutputType", GetOutputType(), propertyGroup);
						SetElementValue("DefineConstants", GetFlattenStringList(PreprocessorDefinitions), propertyGroup);
						SetElementValue("TargetFrameworkVersion", FrameworkVersion.ToString().Replace('_', '.'), propertyGroup);
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
						AddStringListToEllementAsAttribute(referencesItemGroup, "Reference", "Include", ReferenceBinaryFiles);

						XmlElement compilesItemGroup = CreateElement("ItemGroup", projectElement);
						AddStringListToEllementAsAttribute(compilesItemGroup, "Compile", "Include", CompileFiles);
					}
				}

				return projectElement.OwnerDocument.OuterXml;
			}
		}

		public virtual string[] ReferenceBinaryFiles
		{
			get { return referenceBinaryFiles.ToArray(); }
		}

		public virtual void AddReferenceBinaryFile(string FilePath)
		{
			referenceBinaryFiles.Add(FilePath);
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
					throw new Exception("[" + OutputType + "] doesn't supported by [" + GetType().Name + "]");
			}

			return type;
		}
	}
}
