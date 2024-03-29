// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

namespace Engine.Frontend.System.Generator
{
	static class DependencyGraphGenerator
	{
		private const ProjectBase.ProfileBase.BuildConfigurations Configuration = ProjectBase.ProfileBase.BuildConfigurations.Debug;
		private const ProjectBase.ProfileBase.PlatformArchitectures Platform = ProjectBase.ProfileBase.PlatformArchitectures.x86;

		private static string FilePath
		{
			get { return EnvironmentHelper.EngineDirectory + "DependencyGraph.dgml"; }
		}

		public static bool Generate()
		{
			RulesLibrary.Instance.Build(false, Configuration, Platform);

			ModuleRules[] modules = RulesLibrary.Instance.GetModuleRules(Configuration, Platform);

			List<string> namespaces = new List<string>();
			foreach (ModuleRules module in modules)
			{
				string ns = module.GetType().Namespace;

				if (namespaces.Contains(ns))
					continue;

				namespaces.Add(ns);
			}

			Action<Action<string, string>> IterateOverNamespaces = (Action<string, string> Add) =>
				{
					foreach (string ns in namespaces)
					{
						int firstDotIndex = ns.IndexOf('.');
						firstDotIndex = ns.IndexOf('.', firstDotIndex + 1);

						while (firstDotIndex != -1 && firstDotIndex < ns.Length)
						{
							int secondDotIndex = ns.IndexOf('.', firstDotIndex + 1);
							if (secondDotIndex == -1)
								secondDotIndex = ns.Length;

							Add(ns.Substring(0, firstDotIndex), ns.Substring(0, secondDotIndex));

							firstDotIndex = secondDotIndex;
						}

						Add("", ns);
					}
				};

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

							node.SetAttribute("Id", module.GetType().FullName);
							node.SetAttribute("Label", module.Name);
						}
					}

					IterateOverNamespaces((string Parent, string Child) =>
					{
						XmlElement node = document.CreateElement("Node");
						{
							nodes.AppendChild(node);

							string[] parts = Child.Split('.');

							node.SetAttribute("Id", Child);
							node.SetAttribute("Label", parts[parts.Length - 1]);
							node.SetAttribute("IsUnreferenced", "True");
							node.SetAttribute("Group", "Expanded");
						}

						if (!string.IsNullOrEmpty(Parent))
						{
							node = document.CreateElement("Node");
							{
								nodes.AppendChild(node);

								string[] parts = Parent.Split('.');

								node.SetAttribute("Id", Parent);
								node.SetAttribute("Label", parts[parts.Length - 1]);
								node.SetAttribute("IsUnreferenced", "True");
								node.SetAttribute("Group", "Expanded");
							}
						}
					});

					//foreach (string ns in namespaces)
					//{
					//	XmlElement node = document.CreateElement("Node");
					//	{
					//		nodes.AppendChild(node);

					//		string[] parts = ns.Split('.');

					//		node.SetAttribute("Id", ns);
					//		node.SetAttribute("Label", parts[parts.Length - 1]);
					//		node.SetAttribute("IsUnreferenced", "True");
					//		node.SetAttribute("Group", "Expanded");
					//	}
					//}
				}

				XmlElement links = document.CreateElement("Links");
				{
					project.AppendChild(links);

					foreach (ModuleRules module in modules)
					{
						string[] dependencies = module.GetAllDependencies();

						Type moduleType = module.GetType();

						XmlElement link = document.CreateElement("Link");
						{
							links.AppendChild(link);

							link.SetAttribute("Source", moduleType.Namespace);
							link.SetAttribute("Target", moduleType.FullName);
							link.SetAttribute("Category", "Contains");
						}

						foreach (string dep in dependencies)
						{
							ModuleRules depModule = Array.Find(modules, (ModuleRules m) => m.Name == dep);

							if (depModule == null)
								throw new FrontendException($"Dependency {dep} doesn't exists");

							link = document.CreateElement("Link");
							{
								links.AppendChild(link);

								link.SetAttribute("Source", moduleType.FullName);
								link.SetAttribute("Target", depModule.GetType().FullName);
							}
						}
					}

					IterateOverNamespaces((string Parent, string Child) =>
					{
						if (string.IsNullOrEmpty(Parent))
							return;

						XmlElement link = document.CreateElement("Link");
						{
							links.AppendChild(link);

							link.SetAttribute("Source", Parent);
							link.SetAttribute("Target", Child);
							link.SetAttribute("Category", "Contains");
						}
					});
				}
			}

			File.WriteAllText(FilePath, document.OuterXml);

			return true;
		}
	}
}