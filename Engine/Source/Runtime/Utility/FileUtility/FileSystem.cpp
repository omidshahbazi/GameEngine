// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	using namespace Platform;

	namespace FileUtility
	{
		bool FileSystem::m_Initialized;
		FileSystem::Fields* FileSystem::m_Fields = nullptr;

		void FileSystem::Initialize(void)
		{
			char16 buffer[MAX_PATH_LENGTH + 1];
			PlatformOS::GetExecutablePath(buffer);

			m_Fields = ReinterpretCast(FileSystem::Fields*, AllocateMemory(RootAllocator::GetInstance(), sizeof(FileSystem::Fields)));
			Construct(m_Fields);

			m_Fields->ExecutablePath = buffer;

			m_Fields->WorkingPath = m_Fields->ExecutableDirectory = Path::GetDirectoryName(m_Fields->ExecutablePath);

			m_Initialized = true;
		}

		void FileSystem::Deinitialize(void)
		{
			Destruct(m_Fields);
			DeallocateMemory(RootAllocator::GetInstance(), m_Fields);
		}

		const WString& FileSystem::GetExecutablePath(void)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			return m_Fields->ExecutablePath;
		}

		const WString& FileSystem::GetExecutableDirectory(void)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			return m_Fields->ExecutableDirectory;
		}

		void FileSystem::SetWorkingPath(const WString& Value)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			m_Fields->WorkingPath = Path::Normalize(Value);
		}

		const WString& FileSystem::GetWorkingPath(void)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			return m_Fields->WorkingPath;
		}

		void FileSystem::Delete(const WString& Path)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			PlatformDirectory::Delete(Path.GetValue());
		}

		bool FileSystem::Exists(const WString& Path)
		{
			HardAssert(m_Initialized, "FileSystem not initialized");

			return PlatformDirectory::Exists(Path.GetValue());
		}

		void FileSystem::GetFiles(const WString& Path, WStringList& Files, SearchOptions SearchOption)
		{
			auto fileIT = PlatformDirectory::GetFiles(Path.GetValue());

			for (; fileIT != PlatformDirectory::DirectoryIterator(); ++fileIT)
				Files.Add(fileIT.GetPath());

			if (SearchOption == SearchOptions::All)
			{
				auto dirIT = PlatformDirectory::GetDirectories(Path.GetValue());

				for (; dirIT != PlatformDirectory::DirectoryIterator(); ++dirIT)
					GetFiles(dirIT.GetPath(), Files, SearchOption);
			}
		}

		void FileSystem::GetFiles(const WString& Path, WStringList& Files, const WString& Pattern, SearchOptions SearchOption)
		{
			auto fileIT = PlatformDirectory::GetFiles(Path.GetValue(), Pattern.GetValue());

			for (; fileIT != PlatformDirectory::DirectoryIterator(); ++fileIT)
				Files.Add(fileIT.GetPath());

			if (SearchOption == SearchOptions::All)
			{
				auto dirIT = PlatformDirectory::GetDirectories(Path.GetValue());

				for (; dirIT != PlatformDirectory::DirectoryIterator(); ++dirIT)
					GetFiles(dirIT.GetPath(), Files, Pattern, SearchOption);
			}
		}

		uint64 FileSystem::GetSize(const WString& Path)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

			if (handle == 0)
				return 0;

			uint64 size = PlatformFile::Size(handle);

			PlatformFile::Close(handle);

			return size;
		}

		bool FileSystem::ReadAllBytes(const WString& Path, byte* Data, uint64& Length)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

			if (handle == 0)
				return false;

			Length = PlatformFile::Read(handle, Data, Length);

			PlatformFile::Close(handle);

			return true;
		}

		bool FileSystem::WriteAllBytes(const WString& Path, const byte* Data, uint64 Length)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Output);

			if (handle == 0)
				return false;

			PlatformFile::Write(handle, Data, Length);

			PlatformFile::Close(handle);

			return true;
		}

		bool FileSystem::ReadAllText(const WString& Path, String* Data)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);

			*Data = String((uint32)size);

			if (handle == 0)
				return false;

			size = PlatformFile::Read(handle, Data->GetValue(), size);

			*Data = String(Data->GetValue(), size);

			PlatformFile::Close(handle);

			return true;
		}

		bool FileSystem::ReadAllText(const WString& Path, WString* Data)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);

			*Data = WString((uint32)size);

			if (handle == 0)
				return false;

			size = PlatformFile::Read(handle, Data->GetValue(), size);

			*Data = WString(Data->GetValue(), size);

			PlatformFile::Close(handle);

			return true;
		}

		bool FileSystem::WriteAllText(const WString& Path, const WString& Data)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output);

			if (handle == 0)
				return false;

			PlatformFile::Write(handle, Data.GetValue(), Data.GetLength());

			PlatformFile::Close(handle);

			return true;
		}
	}
}