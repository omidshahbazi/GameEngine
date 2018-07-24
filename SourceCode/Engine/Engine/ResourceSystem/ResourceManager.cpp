// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformOS.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>

namespace Engine
{
	using namespace Utility;
	using namespace Utility::YAML;
	using namespace Platform;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString META_EXTENSION(L".meta");

		template<typename T>
		T *Allocate(uint32 Count)
		{
			return reinterpret_cast<T*>(AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, Count * sizeof(T)));
		}

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

		template<typename T>
		uint64 ReadFileContent(const WString &Path, T **Data)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			if (handle == 0)
				return 0;

			uint64 fileSize = PlatformFile::Size(handle);

			*Data = Allocate<T>(fileSize);

			if ((fileSize = PlatformFile::Read(handle, *Data, fileSize)) == 0)
				return 0;

			PlatformFile::Close(handle);

			return fileSize;
		}

		SINGLETON_DECLARATION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			Compile();

			/*auto fileHandle = PlatformFile::Open((GetWorkingPath() + L"/Test.txt").GetValue(), PlatformFile::OpenModes::Input);

			char8 str[1024];
			PlatformFile::Read(fileHandle, str, 1024);
			PlatformFile::Close(fileHandle);

			YAMLObject obj;
			YAMLParser parser;
			parser.Parse(str, obj);*/

		}

		ResourceManager::~ResourceManager(void)
		{
		}

		Resource *ResourceManager::Load(const WString &Path)
		{
			char8 *data = nullptr;
			uint64 size = ReadFileContent(GetWorkingPath() + L"/" + Path, &data);

			return nullptr;
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