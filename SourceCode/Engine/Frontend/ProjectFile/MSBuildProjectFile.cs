// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Collections.Generic;
using System.Xml;

namespace Engine.Frontend.ProjectFile
{
	abstract class MSBuildProjectFile : ProjectFile
	{
		protected virtual XmlElement ProjectElement
		{
			get
			{
				XmlDocument document = new XmlDocument();

				XmlElement project = CreateElement("Project", document);

				project.SetAttribute("DefaultTargets", "Build");
				project.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

				return project;
			}
		}

		protected virtual XmlElement CreateElement(string Name, XmlNode Parent)
		{
			XmlElement elem = Parent.OwnerDocument.CreateElement(Name);

			Parent.AppendChild(elem);

			return elem;
		}

		protected virtual XmlElement GetOrCreateElement(string Name, XmlNode Parent)
		{
			if (Parent[Name] == null)
				return CreateElement(Name, Parent);

			return Parent[Name];
		}

		protected virtual XmlElement SetElementValue(string Name, string Value, XmlNode Parent)
		{
			XmlElement elem = GetOrCreateElement(Name, Parent);
			elem.InnerText = Value;
			return elem;
		}

		protected static string GetFlattenStringList(IEnumerable<string> List)
		{
			return GetFlattenStringList(List, ";");
		}

		protected static void AddStringListToEllementAsAttribute(XmlElement Parent, string ElementName, string AttributeName, IEnumerable<string> List)
		{
			IEnumerator<string> it = List.GetEnumerator();

			while (it.MoveNext())
			{
				XmlElement elem = CreateElement("Reference", Parent);
				elem.SetAttribute("Include", it.Current);
			}

			return value.ToString();

		}
	}
}
