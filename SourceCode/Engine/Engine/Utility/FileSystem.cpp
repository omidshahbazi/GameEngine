// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Platform\PlatformOS.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformDirectory.h>

namespace Engine
{
	using namespace Platform;

	namespace Utility
	{
		bool FileSystem::m_Initialized;
		WString FileSystem::m_ExecutablePath;
		WString FileSystem::m_WorkingPath;

		const WString &FileSystem::GetExecutablePath(void)
		{
			if (!m_Initialized)
				Initialize();

			return m_ExecutablePath;
		}

		void FileSystem::SetWorkingPath(const WString & Value)
		{
			if (!m_Initialized)
				Initialize();

			m_WorkingPath = Path::Normalize(Value);
		}

		const WString &FileSystem::GetWorkingPath(void)
		{
			if (!m_Initialized)
				Initialize();

			return m_WorkingPath;
		}

		void FileSystem::GetFiles(const WString &Path, Vector<WString> &Files, SearchOptions SearchOption)
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