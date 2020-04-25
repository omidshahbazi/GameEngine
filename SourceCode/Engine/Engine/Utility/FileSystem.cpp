// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Platform;

	namespace Utility
	{
		bool FileSystem::m_Initialized;
		WString FileSystem::m_ExecutablePath;
		WString FileSystem::m_WorkingPath;

		const WString& FileSystem::GetExecutablePath(void)
		{
			if (!m_Initialized)
				Initialize();

			return m_ExecutablePath;
		}

		void FileSystem::SetWorkingPath(const WString& Value)
		{
			if (!m_Initialized)
				Initialize();

			m_WorkingPath = Path::Normalize(Value);
		}

		const WString& FileSystem::GetWorkingPath(void)
		{
			if (!m_Initialized)
				Initialize();

			return m_WorkingPath;
		}

		void FileSystem::GetFiles(const WString& Path, FileList& Files, SearchOptions SearchOption)
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

		uint64 FileSystem::GetSize(const WString& Path)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

			if (handle == 0)
				return 0;

			uint64 size = PlatformFile::Size(handle);

			PlatformFile::Close(handle);

			return size;
		}

		void FileSystem::ReadAllBytes(const WString& Path, byte* Data, uint64 Count)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

			if (handle == 0)
				return;

			PlatformFile::Read(handle, Data, Count);

			PlatformFile::Close(handle);
		}

		void FileSystem::WriteAllBytes(const WString& Path, const byte* Data, uint64 Count)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Output);

			if (handle == 0)
				return;

			PlatformFile::Write(handle, Data, Count);

			PlatformFile::Close(handle);
		}

		void FileSystem::ReadAllText(const WString& Path, WString* Data)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);
			*Data = WString(size);

			if (handle == 0)
				return;

			PlatformFile::Read(handle, Data->GetValue(), size);

			PlatformFile::Close(handle);
		}

		void FileSystem::WriteAllText(const WString& Path, const WString& Data)
		{
			PlatformFile::Handle handle = PlatformFile::Open(Path.GetValue(), PlatformFile::OpenModes::Output);

			if (handle == 0)
				return;

			PlatformFile::Write(handle, Data.GetValue(), Data.GetLength());

			PlatformFile::Close(handle);
		}

		void FileSystem::Initialize(void)
		{
			char16 buffer[MAX_PATH_LENGTH + 1];
			PlatformOS::GetExecutablePath(buffer);

			m_ExecutablePath = buffer;

			m_WorkingPath = Path::GetDirectoryName(m_ExecutablePath);

			m_Initialized = true;
		}
	}
}