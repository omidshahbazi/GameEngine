// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Common\BitwiseUtils.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
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

#define SET_WORKING_PATH(Path) \
		cwstr __CurrentWorkingPathStr; \
		PlatformDirectory::GetWokringDirectory(&__CurrentWorkingPathStr); \
		WString __CurrentWorkingPath(__CurrentWorkingPathStr); \
		PlatformDirectory::SetWokringDirectory(Path);

#define REVERT_WORKING_PATH() //PlatformDirectory::SetWokringDirectory(__CurrentWorkingPath.GetValue());

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");
		const WString META_EXTENSION(L".meta");
		const WString DATA_EXTENSION(L".data");
		const String KEY_GUID("GUID");
		const String KEY_LAST_WRITE_TIME("LastWriteTime");
		const String KEY_FILE_FORMAT_VERSION("FileFormatVersion");

		const int8 FILE_FORMAT_VERSION = 1;

		const WString &GetAssetsPath(void)
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

		const WString &GetLibraryPath(void)
		{
			static bool initialized = false;
			static WString path;

			if (!initialized)
			{
				path = Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME);
				initialized = true;
			}

			return path;
		}

		void CheckDirectories(void)
		{
			WString dir = GetAssetsPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir = GetLibraryPath();
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());
		}

		const String &GenerateUUID(void)
		{
			str uuid;
			PlatformOS::GenerateGUID(&uuid);

			static String result;
			result = uuid;

			return result;
		}

		ByteBuffer *ReadDataFile(const WString &Path)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input | PlatformFile::OpenModes::Binary);

			if (handle == 0)
				return nullptr;

			uint64 fileSize = PlatformFile::Size(handle);

			ByteBuffer *buffer = ResourceSystemAllocators::Allocate<ByteBuffer>(1);
			new (buffer) ByteBuffer(&ResourceSystemAllocators::ResourceAllocator, fileSize);

			if ((fileSize = PlatformFile::Read(handle, buffer->GetBuffer(), fileSize)) == 0)
			{
				ResourceSystemAllocators::Deallocate(buffer);

				return nullptr;
			}

			PlatformFile::Close(handle);

			buffer->GetSize() = fileSize;

			return buffer;
		}

		bool WriteDataFile(const WString &Path, ByteBuffer *Buffer)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);

			if (handle == 0)
				return false;

			PlatformFile::Write(handle, Buffer->GetBuffer(), Buffer->GetSize());

			return true;
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
			WStringStream dataFilePathStream;

			ByteBuffer *fileBuffer = ReadDataFile(FilePath);

			*fileBuffer << "asdasds";

			if (fileBuffer == nullptr)
				goto CleanUp;

			ByteBuffer *dataBuffer = ResourceFactory::GetInstance()->Compile(Path::GetExtension(FilePath), fileBuffer);

			if (dataBuffer == nullptr)
				goto CleanUp;

			uint32 hash = Hash::CRC32(FilePath.GetValue(), FilePath.GetLength() * sizeof(WString::CharType));

			dataFilePathStream << "../" << LIBRARY_DIRECTORY_NAME.GetValue() << "/" << hash << DATA_EXTENSION.GetValue();

			bool result = WriteDataFile(dataFilePathStream.str().c_str(), dataBuffer);

		CleanUp:
			if (fileBuffer != nullptr)
			{
				fileBuffer->~Buffer();
				ResourceSystemAllocators::Deallocate(fileBuffer);
			}

			if (dataBuffer != nullptr)
			{
				dataBuffer->~Buffer();
				ResourceSystemAllocators::Deallocate(dataBuffer);
			}

			return result;
		}

		bool ProcessFile(const WString &FilePath)
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

			if (!CompileFile(FilePath))
				return false;

			WriteMetaFile(metaFilePath, obj);

			return true;
		}

		SINGLETON_DECLARATION(ResourceManager)

			ResourceManager::ResourceManager(void)
		{
			ResourceFactory::Create(&ResourceSystemAllocators::ResourceAllocator);

			CheckDirectories();

			Compile();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		Resource *ResourceManager::Load(const String &Path)
		{
			return Load(Path.ChangeType<char16>());
		}

		Resource *ResourceManager::Load(const WString &Path)
		{
			WString finalPath = Path.ToLower();

			SET_WORKING_PATH(GetLibraryPath().GetValue());

			ByteBuffer *buffer = ReadDataFile(finalPath);

			if (buffer == nullptr)
				return nullptr;

			Resource *resource = ResourceFactory::GetInstance()->Create(buffer);

			buffer->~Buffer();
			ResourceSystemAllocators::Deallocate(buffer);

			REVERT_WORKING_PATH();

			return resource;
		}

		void ResourceManager::Compile(void)
		{
			WString assetsPath = GetAssetsPath();

			SET_WORKING_PATH(assetsPath.GetValue());

			Vector<WString> files;
			FileSystem::GetFiles(assetsPath, files, FileSystem::SearchOptions::All);

			for each (const auto &path in files)
			{
				if (Path::GetExtension(path).ToLower() == META_EXTENSION)
					continue;

				WString finalPath = path.SubString(assetsPath.GetLength() + 1).ToLower();

				ProcessFile(finalPath);
			}

			REVERT_WORKING_PATH();
		}
	}
}