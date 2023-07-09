// Copyright 2016-2020 ?????????????. All Rights Reserved.
using GameFramework.ASCIISerializer;
using System.IO;
using System.Text;

namespace Engine.Frontend.System.Generator
{
	static class RenderDocSettingsGenerator
	{
		public static void Generate(string outputPath, string executablePath, string workingDirectory, params string[] commandLineParameters)
		{
			ISerializeObject rootObj = Creator.Create<ISerializeObject>();
			{
				rootObj["rdocCaptureSettings"] = 1;

				ISerializeObject settingsObj = rootObj.AddObject("settings");
				{
					settingsObj["executable"] = executablePath;
					settingsObj["workingDir"] = workingDirectory;

					settingsObj["autoStart"] = false;

					StringBuilder cmdBuilder = new StringBuilder();
					foreach (string cmd in commandLineParameters)
					{
						cmdBuilder.Append(cmd);
						cmdBuilder.Append(' ');
					}

					settingsObj["commandLine"] = cmdBuilder.ToString();
					settingsObj["inject"] = false;
					settingsObj["numQueuedFrames"] = 0;
					settingsObj["queuedFrameCap"] = 0;

					ISerializeArray environmentArr = settingsObj.AddArray("environment");
					{
					}

					ISerializeObject optionsObj = settingsObj.AddObject("options");
					{
						optionsObj["allowFullscreen"] = true;
						optionsObj["allowVSync"] = true;
						optionsObj["apiValidation"] = true;
						optionsObj["captureAllCmdLists"] = true;
						optionsObj["captureCallstacks"] = true;
						optionsObj["captureCallstacksOnlyDraws"] = false;
						optionsObj["debugOutputMute"] = true;
						optionsObj["delayForDebugger"] = 0;
						optionsObj["hookIntoChildren"] = false;
						optionsObj["refAllResources"] = true;
						optionsObj["verifyBufferAccess"] = true;
					}
				}
			}

			File.WriteAllText(outputPath, rootObj.Content);
		}
	}
}