// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformOS.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\YAML\YAMLObject.h>
#include <Utility\YAML\YAMLArray.h>


#include <Utility\Lexer\Lexer.h>

namespace Engine
{
	using namespace Utility;
	using namespace Utility::YAML;
	using namespace Platform;

	namespace ResourceSystem
	{
		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString META_EXTENSION(L".meta");

		const WString &GetWorkingPath(void)
		{
			static bool initialized = false;
			static WString path;

			if (!initialized)
			{
				path = Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME);
				initialized = true;
			}

			return path;
		}

		const String &GenerateUUID(void)
		{
			str uuid;
			PlatformOS::GenerateGUID(&uuid);

			static String result;
			result = uuid;

			return result;
		}

		SINGLETON_DECLARATION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			Compile();

			//YAMLObject obj;
			//obj["Bool"] = true;
			//obj["Int32"] = 5046;


			//YAMLArray arr1;
			//obj["Arr"] = &arr1;
			//arr1.Add(false);
			//arr1.Add(true);
			//arr1.Add(false);


			//YAMLObject obj1;
			//arr1.Add(&obj1);

			//obj1["Drieved1"] = "1asdasdas2";

			//auto fileHandle = PlatformFile::Open((GetWorkingPath() + L"/Test.txt").GetValue() , PlatformFile::OpenModes::Output);
			//PlatformFile::Write(fileHandle, obj.ToString().GetValue());
			//PlatformFile::Close(fileHandle);

			auto fileHandle = PlatformFile::Open((GetWorkingPath() + L"/Test.txt").GetValue(), PlatformFile::OpenModes::Input);

			char8 str[1024];
			PlatformFile::Read(fileHandle, str, 1024);
			PlatformFile::Close(fileHandle);

			Lexer::Lexer lex(str);

			while (true)
			{
				Lexer::Token token = lex.ReadNextToken();

				if (token.GetType() == Lexer::Token::Types::End)
					break;
			}
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		void ResourceManager::Compile(void)
		{
			WString workingPath = GetWorkingPath();

			Vector<WString> files;
			FileSystem::GetFiles(workingPath, files, FileSystem::SearchOptions::All);

			for each (const auto &path in files)
			{
				if (Path::GetExtension(path) == META_EXTENSION)
					continue;

				WString metaFilePath = path + META_EXTENSION;
				uint64 lastWriteTime = PlatformFile::GetLastWriteTime(path.GetValue());

				YAMLObject obj;

				if (PlatformFile::Exists(metaFilePath.GetValue()))
				{
					if (lastWriteTime == obj["LastWriteTime"].GetAsInt64())
						continue;


				}
				else
					auto fileHandle = PlatformFile::Open(metaFilePath.GetValue(), PlatformFile::OpenModes::Output);
			}
		}
	}
}