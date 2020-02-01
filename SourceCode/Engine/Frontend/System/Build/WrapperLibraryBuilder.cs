// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Build
{
	class WrapperLibraryBuilder : BuilderBase
	{
		private struct TypedefInfo
		{
			public string Name;
			public string[] Arguments;

			public string[] Parameters;

			public int StartIndex;
			public int EndIndex;

			public string BaseBody;
		}

		public class WrapperFileList : List<string>
		{ }

		private const string ProjectName = "Wrapper";
		private const string NewLine = "\r\n";
		private const string TemplateMarkup = "//template ";
		private const string TypedefMarkup = "//typedef ";

		private string allWrapperContent = "";
		private WrapperFileList wrapperFiles = null;

		public WrapperLibraryBuilder() : base(ProjectName)
		{
			wrapperFiles = new WrapperFileList();
		}

		public override bool Build(bool ForceToRebuild)
		{
			foreach (string file in wrapperFiles)
				ProcessForTemplateTypedefs(file);

			CSProject csproj = new CSProject();
			CSProject.Profile profile = (CSProject.Profile)csproj.CreateProfile();

			profile.FrameworkVersion = CSProject.Profile.FrameworkVersions.v4_5;
			profile.AssemblyName = ProjectName;
			profile.OutputPath = BinariesPath;
			profile.IntermediatePath = IntermediateModulePath;
			profile.OutputType = ProjectBase.ProfileBase.OutputTypes.DynamicLinkLibrary;
			profile.PlatformArchitecture = BuildSystem.PlatformArchitecture;

			DateTime startTime = DateTime.Now;
			ConsoleHelper.WriteInfo("Building wrapper starts at " + startTime.ToString());

			if (wrapperFiles.Count == 0)
			{
				ConsoleHelper.WriteInfo("No building rules found, aborting process");
				return false;
			}

			foreach (string file in wrapperFiles)
				csproj.AddCompileFile(file);

			if (Compiler.Build(profile))
			{
				CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				ConsoleHelper.WriteInfo("Building wrapper takes " + (DateTime.Now - startTime).ToHHMMSS());

				return true;
			}

			return false;
		}

		public void AddWrapperFile(string FilePath)
		{
			if (!File.Exists(FilePath))
				return;

			allWrapperContent += File.ReadAllText(FilePath);

			wrapperFiles.Add(FilePath);
		}

		private void ProcessForTemplateTypedefs(string FilePath)
		{
			string content = File.ReadAllText(FilePath);

			bool isDirty = false;

			int fromIndex = 0;
			while (true)
			{
				fromIndex = content.IndexOf(TypedefMarkup, fromIndex);

				if (fromIndex == -1)
					break;

				TypedefInfo info;

				if (!ReadTypedefInfo(content, fromIndex, out info))
				{
					++fromIndex;

					continue;
				}

				content = content.Insert(info.StartIndex, "//");
				info.EndIndex += 2;

				string classDeclaration = "public class " + info.Name + NewLine;

				content = content.Insert(info.EndIndex, classDeclaration);
				info.EndIndex += classDeclaration.Length;

				for (int i = 0; i < info.Arguments.Length; ++i)
					info.BaseBody = info.BaseBody.Replace(info.Parameters[i] + " ", info.Arguments[i] + " ");

				content = content.Insert(info.EndIndex, info.BaseBody);
				info.EndIndex += info.BaseBody.Length;

				content = content.Insert(info.EndIndex, NewLine);
				info.EndIndex += NewLine.Length;

				fromIndex = info.EndIndex;

				isDirty = true;
			}

			if (isDirty)
				File.WriteAllText(FilePath, content);
		}

		private bool ReadTypedefInfo(string Content, int FromIndex, out TypedefInfo Info)
		{
			Info = new TypedefInfo();

			string line = ReadTillNewLine(Content, ref FromIndex);
			if (line == null)
				return false;

			string[] parts = line.Split(' ');

			if (parts.Length != 3)
				return false;

			Info.Name = parts[1];
			string baseName = parts[2];
			List<string> arguments = new List<string>();

			int prevIndex = 0;
			while (true)
			{
				prevIndex = FromIndex;

				string parameter = ReadTillNewLine(Content, ref FromIndex);
				if (parameter == null)
					return false;

				if (!parameter.StartsWith("//"))
				{
					FromIndex = prevIndex;
					break;
				}

				arguments.Add(parameter.Substring(2));
			}

			Info.Arguments = arguments.ToArray();

			Info.StartIndex = FromIndex;
			line = ReadTillNewLine(Content, ref FromIndex);
			if (line == null)
				return false;
			Info.EndIndex = FromIndex;

			FromIndex = allWrapperContent.IndexOf(TemplateMarkup + baseName);
			
			line = ReadTillNewLine(allWrapperContent, ref FromIndex);
			if (line == null)
				return false;

			line = ReadTillNewLine(allWrapperContent, ref FromIndex);
			if (line == null)
				return false;

			int lessThanIndex = line.IndexOf('<');
			if (lessThanIndex == -1)
				return false;
			++lessThanIndex;

			int greaterThanIndex = line.IndexOf('>', lessThanIndex);
			if (greaterThanIndex == -1)
				return false;

			Info.Parameters = line.Substring(lessThanIndex, greaterThanIndex - lessThanIndex).Split(',');
			if (Info.Arguments.Length != Info.Parameters.Length)
				return false;

			Info.BaseBody = ReadTillEndOfScope(allWrapperContent, FromIndex);
			if (Info.BaseBody == null)
				return false;

			return true;
		}

		private string ReadTillNewLine(string Content, ref int FromIndex)
		{
			int newLineIndex = Content.IndexOfAny(NewLine.ToCharArray(), FromIndex);

			if (newLineIndex == -1)
				return null;

			string result = Content.Substring(FromIndex, newLineIndex - FromIndex);

			FromIndex = newLineIndex;

			while (FromIndex < Content.Length)
			{
				if (Content[FromIndex] == NewLine[0] ||
					Content[FromIndex] == NewLine[1])
				{
					++FromIndex;
					continue;
				}

				break;
			}

			return result;
		}

		private string ReadTillEndOfScope(string Content, int FromIndex)
		{
			FromIndex = Content.IndexOf('{', FromIndex);
			if (FromIndex == -1)
				return null;

			int endIndex = FromIndex;
			int scopeIndent = 1;
			while (scopeIndent != 0)
			{
				++endIndex;

				if (endIndex >= Content.Length)
					return null;

				char c = Content[endIndex];

				if (c == '{')
				{
					++scopeIndent;
					continue;
				}

				if (c == '}')
				{
					--scopeIndent;
					continue;
				}
			}

			++endIndex;

			return Content.Substring(FromIndex, endIndex - FromIndex);
		}
	}
}