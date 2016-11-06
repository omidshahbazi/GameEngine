// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;
using System.Xml;

namespace Engine.Frontend
{
	//https://msdn.microsoft.com/en-us/library/dd576348.aspx

	class CSProjectFileGenerator : MSBuildProjectFileGenerator
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

		private XmlElement propertyGroup = null;
		private XmlElement referencesItemGroup = null;
		private XmlElement compilesItemGroup = null;

		public override string AssemblyName
		{
			get { return propertyGroup["AssemblyName"].InnerText; }
			set { SetElementValue("AssemblyName", value, propertyGroup); }
		}

		public override string OutputPath
		{
			get { return propertyGroup["OutputPath"].InnerText; }
			set { SetElementValue("OutputPath", value, propertyGroup); }
		}

		public override OutputTypes OutputType
		{
			get
			{
				string type = propertyGroup["OutputPath"].InnerText;

				if (type == "Exe")
					return OutputTypes.Application;
				else
					return OutputTypes.DynamicLinkLibrary;
			}
			set
			{
				string type = "";

				if (value == OutputTypes.Application)
					type = "Exe";
				else if (value == OutputTypes.DynamicLinkLibrary)
					type = "Library";
				else
					throw new Exception(value + " doesn't supported by " + GetType().Name);

				SetElementValue("OutputType", type, propertyGroup);
			}
		}

        public override string PreprocessorDefinitions
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

		public FrameworkVersions FrameworkVersion
		{
			get
			{
				string version = propertyGroup["TargetFrameworkVersion"].InnerText;

				if (string.IsNullOrEmpty(version))
					return FrameworkVersions.v2_0;

				return (FrameworkVersions)Enum.Parse(typeof(FrameworkVersions), version.Replace('.', '_'));
			}
			set { SetElementValue("TargetFrameworkVersion", value.ToString().Replace('_', '.'), propertyGroup); }
		}

		public CSProjectFileGenerator()
		{
			propertyGroup = CreateElement("PropertyGroup");
			referencesItemGroup = CreateElement("ItemGroup");
			compilesItemGroup = CreateElement("ItemGroup");

			XmlElement target = CreateElement("Target");
			target.SetAttribute("Name", "Build");

			XmlElement elem = CreateElement("MakeDir", target);
			elem.SetAttribute("Directories", "$(OutputPath)");
			elem.SetAttribute("Condition", "!Exists('$(OutputPath)')");

			elem = CreateElement("Csc", target);
			elem.SetAttribute("Sources", "@(Compile)");
			elem.SetAttribute("OutputAssembly", "$(OutputPath)$(AssemblyName)");

			target = CreateElement("Target");
			target.SetAttribute("Name", "Clean");

			elem = CreateElement("Delete", target);
			elem.SetAttribute("Files", "$(OutputPath)$(AssemblyName)");

			target = CreateElement("Target");
			target.SetAttribute("Name", "Rebuild");
			target.SetAttribute("DependsOnTargets", "Clean;Build");

			XmlElement import = CreateElement("Import");
			import.SetAttribute("Project", "$(MSBuildToolsPath)/Microsoft.CSharp.targets");
		}

		public void AddReferenceBinaryFile(string FilePath)
		{
			XmlElement elem = CreateElement("Reference", referencesItemGroup);
			elem.SetAttribute("Include", FilePath);
		}

		public override void AddCompileFile(string FilePath)
		{
			XmlElement elem = CreateElement("Compile", compilesItemGroup);
			elem.SetAttribute("Include", FilePath);
		}
	}
}
