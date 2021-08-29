// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Build;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace Engine.Frontend.System.Generator
{
	static class DependencyGraphGenerator
	{
		private static string FilePath
		{
			get { return EnvironmentHelper.EngineDirectory + "DependencyGraph.dgml"; }
		}

		public static bool Generate()
		{
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;

			List<ModuleRules> modules = new List<ModuleRules>();

			NewModuleRuleEventHandler newModuleCallback = (rule) =>
			{
				modules.Add(rule);
			};

			rulesBuilder.OnNewModuleRule += newModuleCallback;

			if (!rulesBuilder.Build(false))
			{
				rulesBuilder.OnNewModuleRule -= newModuleCallback;
				return false;
			}

			rulesBuilder.OnNewModuleRule -= newModuleCallback;

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

				XmlElement styles = document.CreateElement("Styles");
				{
					project.AppendChild(styles);

					XmlElement styleUnreferenced = document.CreateElement("Style");
					{
						styles.AppendChild(styleUnreferenced);

						styleUnreferenced.SetAttribute("TargetType", "Node");
						styleUnreferenced.SetAttribute("GroupLabel", "Unreferenced");
						styleUnreferenced.SetAttribute("ToolTip", "Node has no links pointing to it from other nodes in the graph");
						styleUnreferenced.SetAttribute("ValueLabel", "True");

						XmlElement styleCondition = document.CreateElement("Condition");
						{
							styleUnreferenced.AppendChild(styleCondition);

							styleCondition.SetAttribute("Expression", "IsUnreferenced");
						}

						XmlElement styleSetter = document.CreateElement("Setter");
						{
							styleUnreferenced.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "Background");
							styleSetter.SetAttribute("Value", "Purple");
						}
					}

					XmlElement styleHub = document.CreateElement("Style");
					{
						styles.AppendChild(styleHub);

						styleHub.SetAttribute("TargetType", "Node");
						styleHub.SetAttribute("GroupLabel", "Hub");
						styleHub.SetAttribute("ToolTip", "Node is in the top 10% of highly connected nodes");
						styleHub.SetAttribute("ValueLabel", "Hub");

						XmlElement styleCondition = document.CreateElement("Condition");
						{
							styleHub.AppendChild(styleCondition);

							styleCondition.SetAttribute("Expression", "HasValue(\"Hub\")");
						}

						XmlElement styleSetter = document.CreateElement("Setter");
						{
							styleHub.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "Background");
							styleSetter.SetAttribute("Value", "#FFFBA8A9");
						}
					}

					XmlElement styleStronglyConnected = document.CreateElement("Style");
					{
						styles.AppendChild(styleStronglyConnected);

						styleStronglyConnected.SetAttribute("TargetType", "Node");
						styleStronglyConnected.SetAttribute("GroupLabel", "Circular References");
						styleStronglyConnected.SetAttribute("ToolTip", "Node is part of a strongly coupled component");

						XmlElement styleCondition = document.CreateElement("Condition");
						{
							styleStronglyConnected.AppendChild(styleCondition);

							styleCondition.SetAttribute("Expression", "IsStronglyConnected");
						}

						XmlElement styleSetter = document.CreateElement("Setter");
						{
							styleStronglyConnected.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "Stroke");
							styleSetter.SetAttribute("Value", "#D02030");
						}

						styleSetter = document.CreateElement("Setter");
						{
							styleStronglyConnected.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "StrokeThickness");
							styleSetter.SetAttribute("Value", "2");
						}
					}

					XmlElement styleCircularReferences = document.CreateElement("Style");
					{
						styles.AppendChild(styleCircularReferences);

						styleCircularReferences.SetAttribute("TargetType", "Node");
						styleCircularReferences.SetAttribute("GroupLabel", "Circular References");
						styleCircularReferences.SetAttribute("ToolTip", "Node is part of a strongly coupled component");

						XmlElement styleCondition = document.CreateElement("Condition");
						{
							styleCircularReferences.AppendChild(styleCondition);

							styleCondition.SetAttribute("Expression", "IsCircularLink");
						}

						XmlElement styleSetter = document.CreateElement("Setter");
						{
							styleCircularReferences.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "Stroke");
							styleSetter.SetAttribute("Value", "#D02030");
						}

						styleSetter = document.CreateElement("Setter");
						{
							styleCircularReferences.AppendChild(styleSetter);

							styleSetter.SetAttribute("Property", "StrokeThickness");
							styleSetter.SetAttribute("Value", "2");
						}
					}
				}

				XmlElement nodes = document.CreateElement("Nodes");
				{
					project.AppendChild(nodes);

					foreach (ModuleRules module in modules)
					{
						XmlElement node = document.CreateElement("Node");
						{
							nodes.AppendChild(node);

							node.SetAttribute("Id", module.Name);
							node.SetAttribute("Label", module.Name);
						}
					}
				}

				XmlElement links = document.CreateElement("Links");
				{
					project.AppendChild(links);

					foreach (ModuleRules module in modules)
					{
						ModuleRules.BuildRulesBase rule = module.BuildRules[0];

						List<string> dependencies = new List<string>();
						if (rule.PrivateDependencyModuleNames != null)
							dependencies.AddRange(rule.PrivateDependencyModuleNames);
						if (rule.PublicDependencyModuleNames != null)
							dependencies.AddRange(rule.PublicDependencyModuleNames);

						foreach (string dep in dependencies)
						{
							XmlElement link = document.CreateElement("Link");
							{
								links.AppendChild(link);

								link.SetAttribute("Source", module.Name);
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