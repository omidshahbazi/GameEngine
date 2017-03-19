// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Collections.Generic;
using System.Xml;

namespace Engine.Frontend.ProjectFile
{
	abstract class MSBuildProjectFileGenerator : ProjectFileGenerator
	{
		private XmlDocument xML = null;
		private XmlElement project = null;

		protected XmlElement Project
		{
			get { return project; }
		}

		public override string Content
		{
			get { return xML.OuterXml; }
		}

		protected MSBuildProjectFileGenerator()
		{
			xML = new XmlDocument();

			project = xML.CreateElement("Project");
			project.SetAttribute("DefaultTargets", "Build");
			project.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

			xML.AppendChild(project);
		}

		protected XmlElement CreateElement(string Name, XmlNode Parent = null)
		{
			XmlElement elem = xML.CreateElement(Name);

			if (Parent == null)
				Parent = project;
			Parent.AppendChild(elem);

			return elem;
		}

		protected XmlElement GetOrCreateElement(string Name, XmlNode Parent = null)
		{
			if (Parent == null)
				Parent = project;

			if (Parent[Name] == null)
				return CreateElement(Name, Parent);

			return Parent[Name];
		}

		protected XmlElement SetElementValue(string Name, string Value, XmlNode Parent = null)
		{
			XmlElement elem = GetOrCreateElement(Name, Parent);
			elem.InnerText = Value;
			return elem;
		}
	}
}
