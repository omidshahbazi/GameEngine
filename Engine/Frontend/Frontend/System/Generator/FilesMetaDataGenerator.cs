// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System.IO;
using System.Collections.Generic;
using Engine.Frontend.Utilities;

namespace Engine.Frontend.System.Generator
{
	static class FilesMetaDataGenerator
	{
		private const string PragmaOnce_Text = "#pragma once";

		public static void Generate(string SearchPath)
		{
			List<string> extensions = new List<string>();
			extensions.AddRange(EnvironmentHelper.HeaderFileSearchPattern);
			extensions.AddRange(EnvironmentHelper.CompileFileSearchPattern);

			IncludeFilesCorrection(SearchPath, extensions.ToArray());

			//extensions.AddRange(EnvironmentHelper.CSharpFileExtensions);
			//GenerateLicenseAgreement(SearchPath, extensions.ToArray());

			//GenerateHeaderGuard(SearchPath, EnvironmentHelper.HeaderFileExtensions);
		}

		private static void GenerateHeaderGuard(string SearchPath, params string[] Extension)
		{
			string[] files = FileSystemUtilites.GetAllFiles(SearchPath, Extension);

			foreach (string filePath in files)
			{
				if (IsForbiddenPath(filePath))
					continue;

				List<string> lines = new List<string>(File.ReadAllLines(filePath));

				string macroName = Path.GetFileName(filePath).ToUpper().Replace('.', '_');

				string controlStatement = $"#ifndef {macroName}";
				string defineStatement = $"#define {macroName}";
				string endStatement = $"#endif // {macroName}";

				int i = Find(controlStatement, lines);
				if (i != 1)
				{
					if (i != -1)
						lines.RemoveAt(i);

					lines.Insert(1, controlStatement);
				}

				i = Find(defineStatement, lines);
				if (i != 2)
				{
					if (i != -1)
						lines.RemoveAt(i);

					lines.Insert(2, defineStatement);
				}

				i = Find(endStatement, lines);
				if (i != lines.Count - 1)
				{
					if (i != -1)
						lines.RemoveAt(i);

					lines.Add(endStatement);
				}

				File.WriteAllLines(filePath, lines.ToArray());
			}
		}

		private static void GenerateLicenseAgreement(string SearchPath, params string[] Extension)
		{
			string[] files = FileSystemUtilites.GetAllFiles(SearchPath, Extension);

			foreach (string filePath in files)
			{
				if (IsForbiddenPath(filePath))
					continue;

				List<string> lines = new List<string>(File.ReadAllLines(filePath));

				int i = Find("// Copyright", lines);
				if (i == 0)
					lines[0] = "// " + EnvironmentHelper.Copyright;
				else
				{
					if (i != -1)
						lines.RemoveAt(i);

					lines.Insert(0, "// " + EnvironmentHelper.Copyright);
				}

				i = Find(PragmaOnce_Text, lines);
				if (i > -1)
					lines.RemoveAt(i);

				File.WriteAllLines(filePath, lines.ToArray());
			}
		}

		private static void IncludeFilesCorrection(string SearchPath, params string[] Extension)
		{
			string[] files = FileSystemUtilites.GetAllFiles(SearchPath, Extension);

			foreach (string filePath in files)
			{
				if (IsForbiddenPath(filePath))
					continue;

				List<string> lines = new List<string>(File.ReadAllLines(filePath));

				for (int i = 0; i < lines.Count; i++)
				{
					string line = lines[i];

					if (!line.StartsWith("#include"))
						continue;

					int index = line.IndexOf('"');
					if (index != -1)
					{
						line = line.Remove(index, 1).Insert(index, "<");

						index = line.IndexOf('"');
						line = line.Remove(index, 1).Insert(index, ">");
					}

					line = line.Replace('/', '\\');

					if (!line.ToLower().Contains("public") && !line.ToLower().Contains("private"))
					{
						index = line.IndexOf('<');

						if (index == -1)
							continue;

						index++;
						string file = line.Substring(index, line.IndexOf('>') - index);

						string[] foundFiles = FileSystemUtilites.GetAllFiles(SearchPath, Path.GetFileName(file));

						if (foundFiles.Length == 0)
							continue;

						string includeFilePath = foundFiles[0];

						if (IsThirdpartyPath(includeFilePath) || IsIntermediatePath(includeFilePath))
							continue;

						bool isPublic = includeFilePath.Contains("Public");

						line = line.Insert(index, (isPublic ? "Public" : "Private") + EnvironmentHelper.PathSeparator);
					}

					lines[i] = line;
				}

				File.WriteAllLines(filePath, lines.ToArray());
			}
		}

		private static int Find(string Content, List<string> Lines)
		{
			for (int i = 0; i < Lines.Count; i++)
				if (Lines[i].StartsWith(Content))
					return i;

			return -1;
		}

		private static bool IsForbiddenPath(string FilePath)
		{
			return (IsThirdpartyPath(FilePath) ||
				IsIntermediatePath(FilePath) ||
				FilePath.Contains("Debug") ||
				FilePath.Contains("Common.h") ||
				FilePath.Contains("ReflectionDefinitions.h"));
		}

		private static bool IsThirdpartyPath(string FilePath)
		{
			return FilePath.Contains("ThirdParty");
		}

		private static bool IsIntermediatePath(string FilePath)
		{
			return FilePath.Contains("Intermediate");
		}
	}
}
