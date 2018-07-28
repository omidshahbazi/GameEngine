// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Common\BitwiseUtils.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformOS.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Utility\Hash.h>
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>

namespace Engine
{
	using namespace Utility;
	using namespace Containers;
	using namespace Utility::YAML;
	using namespace Platform;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString META_EXTENSION(L".meta");
		const String KEY_GUID("GUID");
		const String KEY_LAST_WRITE_TIME("LastWriteTime");
		const String KEY_FILE_FORMAT_VERSION("FileFormatVersion");

		const int8 FILE_FORMAT_VERSION = 1;

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

		Buffer *ReadFileContent(const WString &Path)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input | PlatformFile::OpenModes::Binary);

			if (handle == 0)
				return nullptr;

			uint64 fileSize = PlatformFile::Size(handle);

			Buffer *buffer = ResourceSystemAllocators::Allocate<Buffer>(1);
			new (buffer) Buffer(&ResourceSystemAllocators::ResourceAllocator, fileSize);

			if ((fileSize = PlatformFile::Read(handle, buffer->GetBuffer(), fileSize)) == 0)
			{
				ResourceSystemAllocators::Deallocate(buffer);

				return nullptr;
			}

			PlatformFile::Close(handle);

			buffer->GetSize() = fileSize;

			return buffer;
		}

		void ReadMetaFile(const WString &Path, YAMLObject &Object)
		{
			YAMLParser parser;

			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			static char8 str[1024];
			PlatformFile::Read(handle, str, 1024);

			PlatformFile::Close(handle);

			parser.Parse(str, Object);
		}

		void WriteMetaFile(const WString &Path, YAMLObject &Object)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output);
			PlatformFile::Write(handle, Object.ToString().GetValue());
			PlatformFile::Close(handle);
		}

		bool CompileFile(const WString &FilePath)
		{
			WString metaFilePath = FilePath + META_EXTENSION;
			int64 lastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue());

			YAMLObject obj;

			if (PlatformFile::Exists(metaFilePath.GetValue()))
			{
				ReadMetaFile(metaFilePath, obj);

				if (lastWriteTime == obj[KEY_LAST_WRITE_TIME].GetAsInt64())
					return true;
			}
			else
				obj[KEY_GUID] = GenerateUUID();

			obj[KEY_FILE_FORMAT_VERSION] = FILE_FORMAT_VERSION;
			obj[KEY_LAST_WRITE_TIME] = lastWriteTime;

			uint32 hash = Hash::CRC32(FilePath.GetValue(), FilePath.GetLength() * sizeof(WString::CharType));

			WriteMetaFile(metaFilePath, obj);

			return true;
		}

		SINGLETON_DECLARATION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			ResourceFactory::Create(&ResourceSystemAllocators::ResourceAllocator);

			Compile();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		Resource *ResourceManager::Load(const WString &Path)
		{
			Buffer *buffer = ReadFileContent(GetWorkingPath() + L"/" + Path);

			if (buffer == nullptr)
				return nullptr;

			return ResourceFactory::GetInstance()->Create(buffer);
		}

		Resource *ResourceManager::Load(const String &Path)
		{
			return Load(Path.ChangeType<char16>());
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

				CompileFile(path);
			}
		}
	}
}