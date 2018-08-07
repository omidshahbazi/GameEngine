// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
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

		void ResourceManager::Compile(void)
		{
			WString assetsPath = GetAssetsPath();

			SetAssetsWorkingPath();

			Vector<WString> files;
			FileSystem::GetFiles(assetsPath, files, FileSystem::SearchOptions::All);

			for each (const auto &path in files)
			{
				if (Path::GetExtension(path).ToLower() == META_EXTENSION)
					continue;

				WString finalPath = path.SubString(assetsPath.GetLength() + 1).ToLower();

				ProcessFile(finalPath);
			}

			RevertWorkingPath();
		}

		bool ResourceManager::CompileFile(const WString &FilePath, const WString &DataFilePath)
		{
			ByteBuffer *fileBuffer = ReadDataFile(FilePath);

			if (fileBuffer == nullptr)
				goto CleanUp;

			ByteBuffer *dataBuffer = ResourceFactory::GetInstance()->Compile(Path::GetExtension(FilePath), fileBuffer);

			if (dataBuffer == nullptr)
				goto CleanUp;

			bool result = WriteDataFile(DataFilePath, dataBuffer);

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

		bool ResourceManager::ProcessFile(const WString &FilePath)
		{
			WString metaFilePath = FilePath + META_EXTENSION;
			int64 lastWriteTime = PlatformFile::GetLastWriteTime(FilePath.GetValue());

			WStringStream dataFilePathStream(&ResourceSystemAllocators::ResourceAllocator);
			dataFilePathStream << "../" << LIBRARY_DIRECTORY_NAME << "/" << GetDataFileName(FilePath) << '\0';
			WString dataFilePath = dataFilePathStream.GetBuffer();

			YAMLObject obj;

			if (PlatformFile::Exists(metaFilePath.GetValue()) && PlatformFile::Exists(dataFilePath.GetValue()))
			{
				ReadMetaFile(metaFilePath, obj);

				if (lastWriteTime == obj[KEY_LAST_WRITE_TIME].GetAsInt64())
					return true;
			}
			else
				obj[KEY_GUID] = GenerateUUID();

			obj[KEY_FILE_FORMAT_VERSION] = FILE_FORMAT_VERSION;
			obj[KEY_LAST_WRITE_TIME] = lastWriteTime;

			if (!CompileFile(FilePath, dataFilePath))
				return false;

			WriteMetaFile(metaFilePath, obj);

			return true;
		}


		void ResourceManager::SetAssetsWorkingPath(void)
		{
			SetWorkingPath(GetAssetsPath());
		}

		void ResourceManager::SetLibraryWorkingPath(void)
		{
			SetWorkingPath(GetLibraryPath());
		}

		void ResourceManager::RevertWorkingPath(void)
		{
		}

		void ResourceManager::SetWorkingPath(const WString & Path)
		{
			cwstr currentWorkingPathStr;
			PlatformDirectory::GetWokringDirectory(&currentWorkingPathStr);
			m_LastWorkingPath = currentWorkingPathStr;
			PlatformDirectory::SetWokringDirectory(Path.GetValue());
		}

		WString ResourceManager::GetDataFileName(const WString &FilePath)
		{
			WStringStream stream(&ResourceSystemAllocators::ResourceAllocator);
			uint32 hash = Hash::CRC32(FilePath.GetValue(), FilePath.GetLength() * sizeof(WString::CharType));
			stream << hash << DATA_EXTENSION << '\0';
			return stream.GetBuffer();
		}

		ByteBuffer *ResourceManager::ReadDataFile(const WString &Path)
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

			return buffer;
		}

		bool ResourceManager::WriteDataFile(const WString &Path, ByteBuffer *Buffer)
		{
			auto handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);

			if (handle == 0)
				return false;

			PlatformFile::Write(handle, Buffer->GetBuffer(), Buffer->GetSize());

			PlatformFile::Close(handle);

			return true;
		}
	}
}