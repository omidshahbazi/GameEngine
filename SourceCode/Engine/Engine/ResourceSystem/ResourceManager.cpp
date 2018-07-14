// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformFile.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\YAMLObject.h>

namespace Engine
{
	using namespace Utility;
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

		SINGLETON_DECLARATION(ResourceManager)

		ResourceManager::ResourceManager(void)
		{
			Compile();

			YAMLObject obj;
			obj["Bool"] = true;

			String str = obj.ToString();
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
				WString metaFilePath = path + META_EXTENSION;


				if (PlatformFile::Exists(metaFilePath.GetValue()))
				{

				}
				else
					auto fileHandle = PlatformFile::Open(metaFilePath.GetValue(), PlatformFile::OpenModes::Output);
			}
		}
	}
}