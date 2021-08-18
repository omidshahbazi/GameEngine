// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Build;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace Engine.Frontend.System.Generator
{
	static class DependencyGraphGenerator
	{
		private static string FilePath
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "DependencyGraph.dgml"; }
		}

		public static bool Generate()
		{
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;

			List<BuildRules> rules = new List<BuildRules>();

			NewBuildRuleEventHandler newRuleCallback = (filePath, rule) =>
			{
				rules.Add(rule);
			};

			rulesBuilder.OnNewBuildRule += newRuleCallback;

			if (!rulesBuilder.Build(false))
			{
				rulesBuilder.OnNewBuildRule -= newRuleCallback;
				return false;
			}

			rulesBuilder.OnNewBuildRule -= newRuleCallback;

			CPPProject projectFile = new CPPProject();

			XmlDocument document = new XmlDocument();

			XmlElement project = document.CreateElement("DirectedGraph");
			{
				document.AppendChild(project);

				project.SetAttribute("Layout", "Sugiyama");
				project.SetAttribute("ZoomLevel", "0");
				project.SetAttribute("xmlns", "http://schemas.microsoft.com/vs/2009/dgml");

				XmlElement properties = document.CreateElement("Properties");
				{
					project.AppendChild(properties);

					XmlElement propertyLayout = document.CreateElement("Property");
					{
						properties.AppendChild(propertyLayout);

						propertyLayout.SetAttribute("Id", "Layout");
						propertyLayout.SetAttribute("DataType", "System.String");
					}

					XmlElement propertyZoomLevel = document.CreateElement("Property");
					{
						properties.AppendChild(propertyZoomLevel);

						propertyZoomLevel.SetAttribute("Id", "ZoomLevel");
						propertyZoomLevel.SetAttribute("DataType", "System.String");
					}

					XmlElement propertyLabel = document.CreateElement("Property");
					{
						properties.AppendChild(propertyLabel);

						propertyLabel.SetAttribute("Id", "Label");
						propertyLabel.SetAttribute("DataType", "System.String");
					}
				}

				XmlElement nodes = document.CreateElement("Nodes");
				{
					project.AppendChild(nodes);

					foreach (BuildRules buildRule in rules)
					{
						XmlElement node = document.CreateElement("Node");
						{
							nodes.AppendChild(node);

							node.SetAttribute("Id", buildRule.ModuleName);
							node.SetAttribute("Label", buildRule.ModuleName);
						}
					}
				}

				XmlElement links = document.CreateElement("Links");
				{
					project.AppendChild(links);

					foreach (BuildRules buildRule in rules)
					{
						BuildRules.RuleBase rule = buildRule.Rules[0];
						if (rule.DependencyModulesName != null)
							foreach (string dep in rule.DependencyModulesName)
							{
								XmlElement link = document.CreateElement("Link");
								{
									links.AppendChild(link);

									link.SetAttribute("Source", buildRule.ModuleName);
									link.SetAttribute("Target", dep);
								}
							}
					}
				}
			}

			File.WriteAllText(FilePath, document.OuterXml);

			return true;
		}
	}
}